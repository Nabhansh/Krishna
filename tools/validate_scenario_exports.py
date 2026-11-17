#!/usr/bin/env python3
"""Validate the three airport scenario CSV/JSON exports."""

import argparse
import csv
import json
from collections import defaultdict
from pathlib import Path


EXPECTED_PASSENGERS = {
    "normal": 5,
    "capacity_shock": 12,
    "large_stress": 50_000,
}


def validate_run(run, expected_count, scenario_name, lane_outages, officer_outages):
    summary = run["summary"]
    passengers = run["passengers"]
    assert summary["passengers"] == expected_count, scenario_name
    assert summary["completed"] == expected_count, scenario_name
    assert len(passengers) == expected_count, scenario_name
    assert len({passenger["id"] for passenger in passengers}) == expected_count

    lanes = {lane["lane_id"]: lane["officer_id"] for lane in run["lanes"]}
    assert len(lanes) == summary["staffed_lanes"]
    assert summary["staffed_lanes"] == summary["staffed_officers"]
    intervals = defaultdict(list)
    for passenger in passengers:
        arrival = passenger["arrival_minute"]
        start = passenger["service_start_minute"]
        completion = passenger["completion_minute"]
        assert start >= arrival
        assert completion > start
        assert passenger["wait_minutes"] == start - arrival
        assert passenger["lane_id"] in lanes
        assert lanes[passenger["lane_id"]] == passenger["officer_id"]
        intervals[("lane", passenger["lane_id"])].append((start, completion))
        intervals[("officer", passenger["officer_id"])].append((start, completion))
        assert passenger["lane_id"].startswith("Lane-")
        assert passenger["officer_id"].startswith("Officer-")
        lane_index = int(passenger["lane_id"][5:]) - 1
        officer_index = int(passenger["officer_id"][8:]) - 1
        for outage in lane_outages:
            if (
                outage["lane_index"] == lane_index
                and start < outage["end_minute"]
                and outage["start_minute"] < completion
            ):
                raise AssertionError("service overlaps lane outage")
        for outage in officer_outages:
            if (
                outage["officer_index"] == officer_index
                and start < outage["end_minute"]
                and outage["start_minute"] < completion
            ):
                raise AssertionError("service overlaps officer outage")

    for services in intervals.values():
        services.sort()
        assert all(
            previous[1] <= following[0]
            for previous, following in zip(services, services[1:])
        )
    return {passenger["id"] for passenger in passengers}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path, help="scenario export directory")
    directory = parser.parse_args().directory

    for scenario_name, expected_count in EXPECTED_PASSENGERS.items():
        json_path = directory / f"{scenario_name}.json"
        csv_path = directory / f"{scenario_name}.csv"
        comparison = json.loads(json_path.read_text(encoding="utf-8"))
        assert comparison["correctness_verified"] is True
        assert len(comparison["runs"]) == 3
        if scenario_name == "large_stress":
            assert comparison["synthetic_input"] is True

        passenger_ids = [
            validate_run(
                run,
                expected_count,
                scenario_name,
                comparison["lane_outages"],
                comparison["officer_outages"],
            )
            for run in comparison["runs"]
        ]
        assert passenger_ids[0] == passenger_ids[1] == passenger_ids[2]

        with csv_path.open(newline="", encoding="utf-8") as stream:
            rows = list(csv.reader(stream))
        assert rows and len(rows[0]) == 41
        assert all(len(row) == len(rows[0]) for row in rows)
        print(
            f"{scenario_name}: {expected_count} passengers per run; "
            f"{len(rows) - 1} CSV rows; CSV/JSON and resource intervals valid"
        )


if __name__ == "__main__":
    main()
