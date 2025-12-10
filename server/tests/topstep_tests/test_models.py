import pytest
from server.app.domain.topstep.models import (
    LoginRequest,
    LoginResponse,
    Account,
    AccountSearchResponse,
    Contract,
    ContractSearchResponse
)

def test_login_request_model():
    req = LoginRequest(username="user", api_key="key")
    assert req.username == "user"
    assert req.api_key == "key"

def test_login_response_model():
    resp = LoginResponse(token="abc")
    assert resp.token == "abc"

def test_account_model():
    acc = Account(id=1, name="Test", accountType="Demo")
    assert acc.id == 1
    assert acc.name == "Test"
    assert acc.accountType == "Demo"

def test_account_search_response_model():
    acc = Account(id=1, name="Test", accountType="Demo")
    resp = AccountSearchResponse(accounts=[acc])
    assert len(resp.accounts) == 1
    assert resp.accounts[0].id == 1

def test_contract_model():
    con = Contract(id=100, contractName="NQ", contractLabel="Nasdaq", tickSize=0.25)
    assert con.id == 100
    assert con.contractName == "NQ"
    assert con.tickSize == 0.25

def test_contract_search_response_model():
    con = Contract(id=100, contractName="NQ", contractLabel="Nasdaq", tickSize=0.25)
    resp = ContractSearchResponse(contracts=[con])
    assert len(resp.contracts) == 1
    assert resp.contracts[0].contractName == "NQ"
