import os
import json
from pathlib import Path
from typing import Dict, Any
import openai
import google.generativeai as genai
from server.security.secrets import KeyringBackend

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

TOPSTEP API GUIDELINES:
If the user selects 'Topstep' as the broker, your Python code must call the domain methods in 'server.app.domain.topstep'.

1. **Authentication**:
   Use `server.app.domain.topstep.auth.authenticate` with `LoginRequest(username, password, api_key)`.
   Retrieve credentials from `KeyringBackend` (e.g., `kb.get("QUANUX_TOPSTEP__USERNAME")`).

2. **Market Data**:
   Use `server.app.domain.topstep.contracts.search_contracts(token, "NQ")` to find Contract ID (e.g. `CON.F.US.ENQ.H26`).

3. **Trading**:
   Use `server.app.domain.topstep.orders.send_order(token, order_request)`.

Structure your 'main.py' to login first, then run the strategy loop.

TASK:
Based on the above guidelines and the following User Answers, generate a complete Python Strategy Package.
Return a JSON object where keys are filenames (e.g., 'signal.py') and values are the file content.

Ensure you generate: __init__.py, main.py, signal.py, entry.py, risk.py, sizing.py.

{user_context}
"""
        return prompt

    def generate_strategy(self, answers: Dict[str, Any], api_key: str = None, provider: str = "openai") -> Dict[str, str]:
        """
        Orchestrates the generation flow.
        Providers: "openai" (default), "gemini"
        """
        # 1. Construct Prompt
        prompt = self.construct_prompt(answers)

        kb = KeyringBackend()
        generated_files = {}

        print(f"DEBUG: Generating strategy using provider: {provider}")

        # --- MOCK ---
        if provider == "mock":
            generated_files = {
                "__init__.py": "",
                "main.py": "print('Hello Mock Strategy')",
                "signal.py": "class MockSignal: pass",
                "entry.py": "class MockEntry: pass",
                "risk.py": "class MockRisk: pass",
                "sizing.py": "class MockSizing: pass"
            }

        # --- GOOGLE GEMINI ---
        elif provider == "gemini":
            real_key = api_key or os.getenv("GEMINI_API_KEY")
            if not real_key and kb._keyring:
                real_key = kb.get("QUANUX_GEMINI_API_KEY")
            
            if not real_key:
                raise ValueError("Gemini API Key not found. Please add QUANUX_GEMINI_API_KEY to secrets.")

            genai.configure(api_key=real_key)
            model = genai.GenerativeModel('gemini-3-flash-preview')
            
            try:
                # Force JSON structure in prompt
                json_prompt = prompt + "\n\nIMPORTANT: Output ONLY valid JSON."
                response = model.generate_content(
                    json_prompt,
                    generation_config=genai.types.GenerationConfig(
                        response_mime_type="application/json"
                    )
                )
                
                content = response.text
                if not content:
                    raise ValueError("Received empty response from Gemini")
                
                generated_files = json.loads(content)

            except Exception as e:
                print(f"ERROR: Gemini call failed: {e}")
                raise RuntimeError(f"Strategy generation failed (Gemini): {e}")

        # --- OPENAI (Default) ---
        else:
            # Retrieve Key
            # Prefer argument, then env, then keyring
            real_key = api_key or os.getenv("OPENAI_API_KEY") 
            if not real_key and kb._keyring:
                real_key = kb.get("QUANUX_OPENAI_API_KEY")
            
            if not real_key:
                 raise ValueError("OpenAI API Key not found. Please add it in the Integrations page.")

            client = openai.OpenAI(api_key=real_key)
            
            print(f"DEBUG: Calling OpenAI with Prompt length {len(prompt)}")
            
            try:
                completion = client.chat.completions.create(
                    model="gpt-4o",
                    messages=[
                        {"role": "system", "content": "You are an expert Python algorithmic trading developer. Output only valid JSON."},
                        {"role": "user", "content": prompt}
                    ],
                    response_format={"type": "json_object"}
                )
                content = completion.choices[0].message.content
                if not content:
                    raise ValueError("Received empty response from OpenAI")
                    
                generated_files = json.loads(content)
                
            except Exception as e:
                print(f"ERROR: OpenAI call failed: {e}")
                raise RuntimeError(f"Strategy generation failed: {e}")

        strategy_name = answers.get('naming', 'MyStrategy').replace(" ", "_")
        
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
