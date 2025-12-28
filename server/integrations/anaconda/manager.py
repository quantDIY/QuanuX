import subprocess
import json
import logging
from typing import List, Dict, Optional

logger = logging.getLogger(__name__)

class AnacondaManager:
    def __init__(self, conda_path: str = "conda"):
        self.conda_path = conda_path

    def get_environments(self) -> List[Dict[str, str]]:
        """List all available Conda environments."""
        try:
            result = subprocess.run(
                [self.conda_path, "env", "list", "--json"],
                capture_output=True,
                text=True,
                check=True
            )
            data = json.loads(result.stdout)
            envs = []
            for path in data.get("envs", []):
                name = path.split("/")[-1]
                envs.append({"name": name, "path": path})
            return envs
        except subprocess.CalledProcessError as e:
            logger.error(f"Failed to list conda environments: {e}")
            return []
        except Exception as e:
            logger.error(f"Error accessing conda: {e}")
            return []

    def run_in_env(self, env_name: str, command: List[str]) -> subprocess.CompletedProcess:
        """Run a command within a specific Conda environment."""
        # Using 'conda run -n env_name command'
        full_cmd = [self.conda_path, "run", "-n", env_name, "--no-capture-output"] + command
        logger.info(f"Running command in {env_name}: {' '.join(full_cmd)}")
        return subprocess.run(full_cmd, capture_output=True, text=True)

manager = AnacondaManager()
