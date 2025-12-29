import os
import json
from pathlib import Path
from typing import Dict, Any

class StrategyBuilder:
    def __init__(self, strategies_dir: str = "server/strategies"):
        self.base_dir = Path(strategies_dir)
        self.agents_doc_path = Path("server/agents.md")

    def _read_agent_guidelines(self) -> str:
        if self.agents_doc_path.exists():
            return self.agents_doc_path.read_text()
        return "Guideline file missing."

    def construct_prompt(self, answers: Dict[str, Any]) -> str:
        guidelines = self._read_agent_guidelines()
        
        user_context = "USER ANSWERS:\n"
        for key, value in answers.items():
            user_context += f"- {key}: {value}\n"

        prompt = f"""
{guidelines}

TASK:
Based on the above guidelines and the following User Answers, generate a complete Python Strategy Package.
Return a JSON object where keys are filenames (e.g., 'signal.py') and values are the file content.

Ensure you generate: __init__.py, main.py, signal.py, entry.py, risk.py, sizing.py.

{user_context}
"""
        return prompt

    def generate_strategy(self, answers: Dict[str, Any], api_key: str = None) -> Dict[str, str]:
        """
        Orchestrates the generation flow.
        """
        # 1. Construct Prompt
        prompt = self.construct_prompt(answers)

        # 2. Call AI (Mocked for now)
        # TODO: Integrate valid AI Client using api_key
        print(f"DEBUG: Mocking AI Call with Prompt length {len(prompt)}")
        
        strategy_name = answers.get('naming', 'MyStrategy').replace(" ", "_")
        
        # MOCK RESPONSE
        generated_files = {
            "__init__.py": f"from .main import {strategy_name}",
            "main.py": f"""
from server.strategies.base import CompositeStrategy
from .signal import CustomSignal
from .entry import CustomEntry
from .risk import CustomRisk
from .sizing import CustomSizing

class {strategy_name}(CompositeStrategy):
    def __init__(self):
        super().__init__(
            name="{strategy_name}",
            signal_module=CustomSignal("SignalLogic"),
            entry_module=CustomEntry("EntryLogic"),
            risk_module=CustomRisk("RiskLogic"),
            position_sizing_module=CustomSizing("SizingLogic")
        )
""",
            "signal.py": """
from server.strategies.base import SignalModule, SignalType, BaseParameters

class CustomSignal(SignalModule):
    def define_parameters(self):
        class Params(BaseParameters):
            period: int = 14
        return Params

    def on_bar(self, bar_data):
        # Placeholder Logic
        return SignalType.NEUTRAL
""",
            "entry.py": """
from server.strategies.base import EntryModule

class CustomEntry(EntryModule):
    def generate_entry(self, signal, bar_data):
        return None
""",
            "risk.py": """
from server.strategies.base import RiskModule

class CustomRisk(RiskModule):
    def calculate_risk(self, entry_price, signal, bar_data):
        return {"stop_loss": 0.0, "take_profit": 0.0}
""",
            "sizing.py": """
from server.strategies.base import PositionSizingModule, SignalType

class CustomSizing(PositionSizingModule):
    def define_parameters(self):
        return None

    def calculate_size(self, signal, price, account_equity):
        return 1.0
"""
        }
        
        if answers.get('broker') == 'Topstep':
            topstep_src = Path("server/app/domain/topstep")
            if topstep_src.exists():
                # Ensure topstep directory exists in output by adding files with 'topstep/' prefix
                generated_files["topstep/__init__.py"] = ""
                for file_path in topstep_src.glob("*.py"):
                    if file_path.name == "__init__.py" and file_path.stat().st_size == 0:
                        continue
                    generated_files[f"topstep/{file_path.name}"] = file_path.read_text()

        # 3. Save to Disk
        # Determine category based on type
        strat_type = answers.get('type', 'Full Strategy')
        if strat_type == "Entry":
            category = "entry"
        elif strat_type == "Exit":
            category = "exit"
        else:
            category = "full"

        output_dir = self.base_dir / category / strategy_name
        output_dir.mkdir(parents=True, exist_ok=True)
        
        for filename, content in generated_files.items():
            file_path = output_dir / filename
            # Handle subdirectories (like topstep/)
            file_path.parent.mkdir(parents=True, exist_ok=True)
            file_path.write_text(content.strip())
            
        return {"status": "success", "path": str(output_dir), "files": generated_files}
