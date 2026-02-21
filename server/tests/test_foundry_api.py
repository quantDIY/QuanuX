import pytest
from fastapi import FastAPI
from fastapi.testclient import TestClient
from unittest.mock import AsyncMock

from server.app.routers.foundry import router
from server.app.services.nats import NatsService

@pytest.fixture
def test_client():
    # Isolate the Foundry router from the rest of the QuanuX app (which has broken dependencies)
    app = FastAPI()
    app.include_router(router)
    
    # Mock NATS directly into the app state
    mock_nats = AsyncMock(spec=NatsService)
    app.state.nats = mock_nats
    return TestClient(app), mock_nats

def test_forge_endpoint(test_client):
    client, mock_nats = test_client
    
    payload = {
        "component_type": "indicator",
        "name": "SuperMACD",
        "target_lang": "python",
        "target_version": "3.14"
    }
    
    response = client.post("/api/foundry/forge", json=payload)
    
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "accepted"
    assert "job_id" in data
    
    # Verify NATS publish was called
    mock_nats.publish.assert_called_once()
    args, _ = mock_nats.publish.call_args
    assert args[0] == "sys.foundry.request.forge"
    assert b"SuperMACD" in args[1]

def test_verify_endpoint(test_client):
    client, mock_nats = test_client
    
    payload = {
        "strategy_name": "TestStrategy123"
    }
    
    response = client.post("/api/foundry/verify", json=payload)
    
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "accepted"
    assert "job_id" in data
    
    # Verify NATS publish was called
    mock_nats.publish.assert_called_once()
    args, _ = mock_nats.publish.call_args
    assert args[0] == "sys.foundry.request.verify"
    assert b"TestStrategy123" in args[1]
