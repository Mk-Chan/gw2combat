import json
import pandas as pd
import numpy as np

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
    df = df.join(pd.json_normalize(df["event"].tolist()).add_prefix("event.")).drop(["event"], axis=1)
    df.columns = df.columns.str.replace("event.", "")

    in_combat_filter = \
        ((df["event_type"] == "damage_event") | (
                    (df["event_type"] == "effect_application_event") & (df["effect"].isin(CONDITION_EFFECTS)))) \
        & (df["source_actor"] != "Console")

    combat_df = df[in_combat_filter].reset_index(drop=True)
    damage_df = combat_df[
        (combat_df["event_type"] == "damage_event") & (combat_df["source_actor"] != "Console")] \
        .reset_index(drop=True)
    time_to_first_strike_ms = min(combat_df["time_ms"])
    combat_time_ms = max(combat_df["time_ms"]) - time_to_first_strike_ms
    damage_summary = damage_df.groupby(by=["damage_type", "source_skill"], dropna=False)["damage"] \
        .aggregate([total_damage, mean_damage, count, dps_for_time_ms(combat_time_ms)]) \
        .reset_index() \
        .sort_values(by=["total_damage"], ascending=[False]) \
        .reset_index(drop=True)
    print(damage_summary.to_string(index=False))
    print()
    print(f"Time to First Strike: {time_to_first_strike_ms / 1000.0}s")
    print(f"Combat Time: {combat_time_ms / 1000.0}s")
    print(f"DPS: {(sum(damage_summary['total_damage']) * 1000.0 / combat_time_ms):.2f}")


if __name__ == "__main__":
    main()
