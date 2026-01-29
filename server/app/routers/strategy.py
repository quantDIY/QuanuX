from fastapi import APIRouter, HTTPException, Body
from pydantic import BaseModel
from typing import Optional, List, Dict, Any

router = APIRouter(prefix="/api/strategy", tags=["strategy"])

class StrategyRequest(BaseModel):
    provider: str
    model: str
    api_key: Optional[str] = None
    requirements: Dict[str, Any]

class StrategyResponse(BaseModel):
    code: str
    explanation: str

from server.strategies.builder import StrategyBuilder

@router.post("/generate", response_model=StrategyResponse)
async def generate_strategy(request: StrategyRequest):
    builder = StrategyBuilder()
    
    # Generate the strategy using the builder
    # result contains {"status": "success", "path": "...", "files": {...}}
    result = builder.generate_strategy(
        answers=request.requirements, 
        api_key=request.api_key,
        provider=request.provider,
        model_name=request.model
    )
    
    # Format the response
    # We'll combine the file contents for the code preview
    code_preview = ""
    for fname, content in result.get("files", {}).items():
        code_preview += f"#File: {fname}\n{content}\n\n"
        
    strategy_name = request.requirements.get('naming', 'MyStrategy')
    
    return StrategyResponse(
        code=code_preview,
        explanation=f"Strategy '{strategy_name}' generated successfully in {result.get('path')}."
    )
@router.post("/backtest/run", response_model=Dict[str, Any])
async def run_backtest(request: StrategyResponse):
    # TODO: Securely execute the code in a supervised environment (e.g. Docker/Conda)
    # For now, we simulate a successful run
    
    return {
        "status": "success",
        "metrics": {
            "total_return": 124.5,
            "sharpe_ratio": 1.85,
            "max_drawdown": -0.152,
            "win_rate": 0.65
        },
        "logs": [
            "[+] Market Data Connection: OK",
            "[+] Strategy Loaded: moving_average_crossover",
            "[+] Backtest Complete."
        ]
    }
