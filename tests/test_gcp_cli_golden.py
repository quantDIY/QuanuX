import pytest
from typer.testing import CliRunner
import json
from server.cli.src.quanuxctl.commands.infra_commands import gcp_sql_app

runner = CliRunner()

import re
def strip_ansi(text):
    return re.sub(r'\x1b\[[0-9;]*m', '', text)

def test_golden_validate_success_human():
    """Validates the exact character stream of a successful validation."""
    result = runner.invoke(gcp_sql_app, ["validate", "SELECT level FROM MarketTick LIMIT 1"])
    assert result.exit_code == 0
    # Rich print will add color codes in TTY, strip them for character assertions
    out = strip_ansi(result.stdout)
    assert "SUCCESS: Query is within the approved Phase 1 bounded matrix." in out

def test_golden_validate_success_json():
    """Validates the exact JSON structural schema for successful validation."""
    result = runner.invoke(gcp_sql_app, ["validate", "SELECT level FROM MarketTick LIMIT 1", "--json"])
    assert result.exit_code == 0
    data = json.loads(result.stdout)
    assert set(data.keys()) == {"mode", "status", "query_fingerprint", "rule_surface_version"}
    assert data["mode"] == "validate"
    assert data["status"] == "success"
    assert data["rule_surface_version"] == "tract2_phase1"

def test_golden_rejection_window_human():
    """Validates the exact fail-closed wording for a banned window function."""
    query = "SELECT AVG(bid_price) OVER (PARTITION BY instrument_id) FROM MarketTick"
    result = runner.invoke(gcp_sql_app, ["validate", query])
    assert result.exit_code == 1
    
    out = " ".join(result.stdout.split())
    assert "FATAL: Prototype Matrix Boundary Violation" in out
    assert "Rejected Construct: WindowFunction" in out
    assert "Violated Rule: Window functions are explicitly banned under the Tract 2 Control Spec" in out
    assert "Fallback required: Please execute complex aggregations natively via the BigQuery client." in out

def test_golden_rejection_join_json():
    """Validates the exact machine-readable JSON structure of a banned construct."""
    query = "SELECT a.level FROM MarketTick a JOIN MarketTick b ON a.level = b.level"
    result = runner.invoke(gcp_sql_app, ["validate", query, "--json"])
    assert result.exit_code == 1
    
    data = json.loads(result.stdout)
    assert set(data.keys()) == {"mode", "status", "error_type", "rejected_construct", "violated_rule", "fallback_instruction", "query_fingerprint"}
    assert data["status"] == "error"
    assert data["error_type"] == "TranspilationError"
    assert "JOIN" in data["rejected_construct"]
    assert "Joins are explicitly banned" in data["violated_rule"]
    assert "Fallback required" in data["fallback_instruction"]

def test_golden_execute_dry_run_json():
    """Validates the exact JSON structural schema for successful execute dry-run."""
    result = runner.invoke(gcp_sql_app, ["execute", "SELECT level FROM MarketTick LIMIT 1", "--dry-run", "--json", "--timeout", "42", "--max-rows", "101"])
    assert result.exit_code == 0
    data = json.loads(result.stdout)
    assert set(data.keys()) == {"mode", "status", "query_fingerprint", "rule_surface_version", "bounds", "row_count", "sql"}
    assert data["mode"] == "execute_dry_run"
    assert data["bounds"]["timeout"] == 42
    assert data["bounds"]["max_rows"] == 101
    assert data["row_count"] == 0
