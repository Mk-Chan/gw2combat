import json

import numpy as np
import pandas as pd

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


def main():
    with open("./audit.json") as fp:
        audit_json = json.load(fp)
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


if __name__ == "__main__":
    main()
