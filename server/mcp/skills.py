import os
import yaml
import asyncio
from pathlib import Path
from dataclasses import dataclass
from typing import List, Optional, Dict, Any

@dataclass
class Skill:
    name: str
    description: str
    tool_authority_path: str  # e.g., "server/skills/name/SKILL.md"
    metadata: Optional[Dict[str, Any]] = None

class SkillLoader:
    def __init__(self, skills_dir: str):
        # Resolve to absolute path relative to CWD if necessary, 
        # but store relative path for "tool authority" cleanliness
        self.skills_dir = Path(skills_dir)
        self.base_dir = Path(os.getcwd())

    def _resolve_skill_path(self, skill_name: str) -> Path:
        # Validate skill name safety (no ../, no weird chars)
        if not skill_name.replace("-", "").isalnum():
             raise ValueError("Invalid skill name")
        return (self.base_dir / self.skills_dir / skill_name / "SKILL.md").resolve()

    def list_skills(self) -> List[Skill]:
        """Discover skills by scanning the skills directory."""
        skills = []
        abs_skills_dir = (self.base_dir / self.skills_dir).resolve()
        
        if not abs_skills_dir.exists():
            return []

        for item in abs_skills_dir.iterdir():
            if item.is_dir():
                skill_file = item / "SKILL.md"
                if skill_file.exists():
                    try:
                        content = skill_file.read_text(encoding="utf-8")
                        # Parse frontmatter manually since PyYAML load_all is needed
                        # Identify YAML block
                        if content.startswith("---"):
                            parts = content.split("---", 2)
                            if len(parts) >= 3:
                                frontmatter = yaml.safe_load(parts[1])
                                skills.append(Skill(
                                    name=frontmatter.get("name", item.name),
                                    description=frontmatter.get("description", "No description provided."),
                                    tool_authority_path=str(skill_file.relative_to(self.base_dir)),
                                    metadata=frontmatter.get("metadata")
                                ))
                    except Exception as e:
                        print(f"Error loading skill {item.name}: {e}")
        return skills

    def read_skill(self, skill_name: str) -> str:
        """Read the full content of a skill file."""
        try:
            skill_path = self._resolve_skill_path(skill_name)
            if not skill_path.exists():
                return f"Error: Skill '{skill_name}' not found."
            
            # Security check: Ensure path is within skills_dir
            if not str(skill_path).startswith(str((self.base_dir / self.skills_dir).resolve())):
                return "Error: Access denied."

            return skill_path.read_text(encoding="utf-8")
        except Exception as e:
            return f"Error reading skill: {e}"

    async def run_script(self, skill_name: str, script_name: str, args: List[str] = []) -> str:
        """Execute a script bundled with the skill."""
        try:
            # Resolve script path
            skill_dir = (self.base_dir / self.skills_dir / skill_name).resolve()
            script_dir = skill_dir / "scripts"
            script_path = (script_dir / script_name).resolve()

            # Security checks
            if not script_path.exists():
                return f"Error: Script '{script_name}' not found in skill '{skill_name}'."
            if not str(script_path).startswith(str(skill_dir)):
                 return "Error: Access denied - Script must be within skill directory."

            # Determine executor based on extension
            cmd = []
            if script_name.endswith(".py"):
                cmd = ["python3", str(script_path)] + args
            elif script_name.endswith(".sh"):
                cmd = ["bash", str(script_path)] + args
            else:
                return f"Error: Unsupported script type for '{script_name}'."

            # Run subprocess
            proc = await asyncio.create_subprocess_exec(
                *cmd,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE,
                cwd=str(skill_dir) # Run in skill dir so relative paths work
            )
            stdout, stderr = await proc.communicate()
            
            status = "Success" if proc.returncode == 0 else "Failed"
            output = f"Execution {status}\nStdout:\n{stdout.decode()}\n"
            if stderr:
                output += f"Stderr:\n{stderr.decode()}"
            return output

        except Exception as e:
            return f"Error executing script: {e}"
