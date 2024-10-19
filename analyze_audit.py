import json

import numpy as np
import pandas as pd

import os
import argparse

CONDITION_EFFECTS = (
    "BLEEDING",
    "BURNING",
    "CONFUSION",
    "POISON",
    "TORMENT",

    "BLINDED",
    "CHILLED",
    "CRIPPLED",
    "FEAR",
    "IMMOBILIZED",
    "SLOW",
    "TAUNT",
    "WEAKNESS",
    "VULNERABILITY",
)


def dps_for_time_ms(time_ms: int):
    def dps(series: pd.Series):
        return sum(series) * 1000.0 / time_ms

    return dps


def total_damage(series: pd.Series):
    return sum(series)


def mean_damage(series: pd.Series):
    return np.mean(series)


def count(series: pd.Series):
    return len(series)


def max2(x):
    y = np.sort(x)
    return y[-2] if len(y) > 1 else 0


def main(args):
    with open("./audit.json") as fp:
        audit_json = json.load(fp)
    with open("./resources/encounter.json") as fp:
        encounter_json = json.load(fp)
    build_path = next(obj["build_path"] for obj in encounter_json["actors"])
    if os.path.exists(build_path):
        with open(build_path) as fp:
            build_json = json.load(fp)
    df = pd.read_json(json.dumps(audit_json["tick_events"]), orient="records")
    df = (df.join(pd.json_normalize(df["event"].tolist()).add_prefix("event."))
          .drop(["event"], axis=1))
    df.columns = df.columns.str.replace("event.", "")

    in_combat_filter = \
        ((df["event_type"] == "damage_event") | (
                (df["event_type"] == "effect_application_event") & (
                "effect" in df and df["effect"].isin(CONDITION_EFFECTS)))) \
        & (df["source_actor"] != "Console")

    combat_df = df[in_combat_filter].reset_index(drop=True)
    damage_df = combat_df[combat_df["event_type"] == "damage_event"].reset_index(drop=True)
    time_to_first_strike_ms = 0 if combat_df.empty else min(combat_df["time_ms"])
    combat_time_ms = 0 if combat_df.empty else max(combat_df["time_ms"]) - time_to_first_strike_ms
    golem_hp_updates = \
        df[(df["event_type"] == "combat_stats_update_event") & (df["actor"] == "golem")][
            "updated_health"]

    afk_time_ms = audit_json["afk_ticks_by_actor"]["player"] \
        if "player" in audit_json["afk_ticks_by_actor"] \
        else 0

    damage_summary = None
    if not damage_df.empty:
        damage_summary = damage_df.groupby(by=["damage_type", "source_skill"], dropna=False)[
            "damage"] \
            .aggregate([total_damage, mean_damage, count, dps_for_time_ms(combat_time_ms)]) \
            .reset_index() \
            .sort_values(by=["total_damage"], ascending=[False]) \
            .reset_index(drop=True)
    total_damage_inflicted = 0 \
        if (damage_summary is None or damage_summary.empty) \
        else damage_summary["total_damage"].sum()

    if damage_summary is not None and not damage_summary.empty:
        print(damage_summary.to_string(index=False))
    print()
    print(f"Time to First Strike: {time_to_first_strike_ms / 1000.0}s")
    print(
        f"Remaining Golem HP: {int(min(golem_hp_updates.values))}")
    print(f"Combat Time: {combat_time_ms / 1000.0}s")
    print(f"AFK Time: {afk_time_ms / 1000.0}s")
    if combat_time_ms != 0:
        print(f"DPS: {(total_damage_inflicted * 1000.0 / combat_time_ms):.2f}")

    if args.extra:
        print()

        build_df = pd.DataFrame(build_json["skills"])
        build_df = build_df[build_df["executable"] == True]
        skills_order_df = build_df[["skill_key"]].rename(columns={"skill_key": "skill"}).reset_index(drop=True)
        for key in ["cooldown", "cast_duration"]:
            build_df[key] = build_df[key].str[-1]
        skills_cast_duration_df = build_df[["skill_key", "cast_duration"]].rename(columns={"skill_key": "skill"})
        build_df = build_df[["skill_key", "cooldown", "cast_duration"]]
        build_df = build_df[build_df["cooldown"] > 0]
        build_df["min_cycle_time"] = (build_df["cooldown"] + build_df["cast_duration"]).astype(int)
        build_df = build_df[["skill_key", "min_cycle_time", "cooldown"]]
        build_df = build_df.rename(columns={"skill_key": "skill"})

        available_skills = audit_json["castable_skills_by_actor"]["player"]
        available_skills_df = pd.DataFrame({ "skill": available_skills })
        available_skills_df["off_cooldown"] = "X"

        uncastable_skills = audit_json["uncastable_skills_by_actor"]["player"]
        uncastable_skills_df = pd.DataFrame.from_dict(uncastable_skills, orient='index')
        uncastable_skills_df = uncastable_skills_df.reset_index(names="skill")
        uncastable_skills_df = uncastable_skills_df.drop(['reason', 'remaining_ammo'], axis=1)
        uncastable_skills_df = uncastable_skills_df[uncastable_skills_df["remaining_cooldown"] > 0]

        current_time_ms = 0 if combat_df.empty else max(combat_df["time_ms"])
        skills_filter = ((df["actor"] == "player") & (df["event_type"] == "skill_cast_begin_event"))
        skills_df = df[skills_filter].reset_index(drop=True)
        skills_summary = skills_df.groupby(by=["skill"])["time_ms"].aggregate([max, max2]).reset_index().sort_values(by=["max"], ascending=[True])
        skills_summary["max2"] = skills_summary.apply(lambda row: "" if (row["max2"] == 0) else row["max"] - row["max2"], axis=1) 
        skills_summary["max"] = current_time_ms - skills_summary["max"]
        skills_summary.rename(columns={'max': 'since_last_cast', "max2": "last_cycle_duration"}, inplace=True)

        skills_cycles = skills_df.groupby(by=["skill"])["time_ms"].apply(list).reset_index(name='cycle_times')
        cycles_data = []
        for _, row in skills_cycles.iterrows():
            cycle_data = []
            for index, time in enumerate(row["cycle_times"]):
                if index >= len(row["cycle_times"]) - 1: break
                cycle_data.append(row["cycle_times"][index + 1] - time)
            cycles_data.append(cycle_data)
        skills_cycles["cycle_durations"] = cycles_data
        skills_cycles["cycle_durations"] = skills_cycles["cycle_durations"].apply(lambda x: [] if len(x) > 15 else x)
        skills_cycles["cycle_durations"] = skills_cycles["cycle_durations"].apply(lambda x: ", ".join(str(s) for s in x))
        skills_cycles = skills_cycles[["skill", "cycle_durations"]]

        afk_filter = ((df["actor"] == "player") & (df["event_type"] == "skill_cast_begin_event"))
        afk_df = df[afk_filter]
        afk_df = afk_df[afk_df.apply(lambda row : not skills_cast_duration_df[skills_cast_duration_df["skill"].str.match(row["skill"])].empty, axis=1)]
        afk_df = afk_df[["skill", "event_type", "time_ms", "cast_duration"]]

        afk_data = []
        next_skill_end_time = 0
        for index, row in afk_df.iterrows():
            if next_skill_end_time <= row["time_ms"]:
                time_diff = row["time_ms"] - next_skill_end_time
                if time_diff > 4:
                    row["afk_time"] = int(time_diff)
                    afk_data.append(row)
                next_skill_end_time = row["time_ms"] + row["cast_duration"]
        afk_data_df = pd.DataFrame(afk_data)
        if not afk_data_df.empty:
            print(afk_data_df[["skill", "time_ms", "afk_time"]].to_string(index=False))

        print()

        skills_combined = skills_order_df.merge(skills_summary, on="skill", how="left")
        skills_combined = skills_combined.merge(available_skills_df, on="skill", how="left")
        skills_combined = skills_combined.merge(uncastable_skills_df, on="skill", how="left")
        skills_combined = skills_combined.merge(build_df, on="skill", how="left")
        skills_combined["available_since"] = skills_combined["since_last_cast"] - skills_combined["min_cycle_time"]
        skills_combined["available_since"] = skills_combined.apply(lambda row: None if (row["available_since"] <= 0) else row["available_since"], axis=1) 
        skills_combined = skills_combined.merge(skills_cycles, on="skill", how="left")
        skills_combined = skills_combined.drop(["min_cycle_time", "cooldown"], axis=1)

        for key in ["since_last_cast", "remaining_cooldown", "available_since"]:
            skills_combined[key] = skills_combined[key].apply(lambda x: f'{x:.0f}' if not pd.isnull(x) else '')

        print(skills_combined.fillna("").to_string(index=False))

        print()

        counters_df = pd.DataFrame(audit_json["counter_values"])
        if not counters_df.empty:
            print(counters_df.sort_values(by=["value"], ascending=[False]).to_string(index=False))

        print()

        unique_effects = audit_json["unique_effects_by_actor"]["player"]
        unique_effects_df = pd.DataFrame.from_dict(unique_effects, orient='index')
        unique_effects_df = unique_effects_df.reset_index(names="unique_effects")
        unique_effects_df = unique_effects_df[unique_effects_df["remaining_duration"] < 1000000]
        unique_effects_df = unique_effects_df[~unique_effects_df['unique_effects'].str.endswith(" Active Effect")]
        if not unique_effects_df.empty:
            print(unique_effects_df.sort_values(by=["stacks"], ascending=[False]).to_string(index=False))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Displays basic combat results")
    parser.add_argument("--extra", help="Display advanced combat results", action="store_true")
    args = parser.parse_args()
    main(args)
