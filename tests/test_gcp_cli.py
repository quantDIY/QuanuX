import pytest
from typer.testing import CliRunner
import os
from server.cli.src.quanuxctl.commands.infra_commands import gcp_sql_app

runner = CliRunner()

def test_cli_validate_success():
    result = runner.invoke(gcp_sql_app, ["validate", "SELECT level FROM MarketTick LIMIT 10"])
    assert result.exit_code == 0
    assert "SUCCESS" in result.stdout

def test_cli_validate_banned():
    result = runner.invoke(gcp_sql_app, ["validate", "SELECT a.level FROM MarketTick a JOIN MarketTick b ON a.level = b.level"])
    assert result.exit_code == 1
    assert "FATAL: Prototype Matrix Boundary Violation" in result.stdout
    assert "Joins are explicitly banned" in result.stdout
    assert "Fallback required" in result.stdout

def test_cli_transpile_top_n():
    # Proven `ORDER BY ... LIMIT` which utilizes DuckDB internal TOP_N mapping
    result = runner.invoke(gcp_sql_app, ["transpile", "SELECT level FROM MarketTick ORDER BY level DESC LIMIT 5"])
    assert result.exit_code == 0
    assert "BigQuery Standard SQL" in result.stdout
    assert "SELECT level FROM MarketTick ORDER BY level DESC LIMIT 5" in result.stdout

def test_cli_execute_dry_run():
    result = runner.invoke(gcp_sql_app, ["execute", "SELECT level FROM MarketTick LIMIT 5", "--dry-run"])
    assert result.exit_code == 0
    assert "DRY-RUN" in result.stdout
    assert "Query would execute as" in result.stdout

def test_cli_execute_real(monkeypatch):
    project_id = os.environ.get("GCP_PROJECT_ID")
    
    # If not in ENV, try resolving using the Secrets manager just like the CLI does
    if not project_id:
        try:
            from server.security.secrets import SecretsInterface
            secrets = SecretsInterface()
            project_id = secrets.get_secret("GCP_PROJECT_ID")
            cred = secrets.get_secret("GOOGLE_APPLICATION_CREDENTIALS")
            if cred:
                monkeypatch.setenv("GOOGLE_APPLICATION_CREDENTIALS", cred)
        except Exception:
            pass

    if not project_id:
        pytest.skip("Requires real GCP credentials in OS Env or via `quanuxctl secrets`.")
        
    monkeypatch.setenv("GCP_PROJECT_ID", project_id)

    # Note: MarketTick is a logical AST hook, so for actual BigQuery we must target
    # an existing object. We'll execute a scalar test to prove bounded logic hooks up.
    result = runner.invoke(gcp_sql_app, ["execute", "SELECT 1 as test_col LIMIT 1", "--max-rows", "1"])
    assert result.exit_code == 0
    assert "Bounded execution complete" in result.stdout
    assert "Retrieved 1 rows" in result.stdout
    assert "test_col" in result.stdout
