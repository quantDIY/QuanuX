import httpx
import keyring
from rich.console import Console
from typing import Optional

console = Console()
SERVICE_NAME = "QuanuX"

class VCSProvider:
    def __init__(self, name: str):
        self.name = name.lower()
    
    def get_token(self) -> Optional[str]:
        return keyring.get_password(SERVICE_NAME, f"VCS_{self.name.upper()}_TOKEN")

    def set_token(self, token: str):
        keyring.set_password(SERVICE_NAME, f"VCS_{self.name.upper()}_TOKEN", token)
        console.print(f"[green]Token for {self.name} stored securely.[/green]")

    def create_repo(self, name: str, private: bool) -> Optional[str]:
        raise NotImplementedError

class GitHubProvider(VCSProvider):
    def __init__(self):
        super().__init__("github")
        self.api_url = "https://api.github.com"

    def create_repo(self, name: str, private: bool) -> Optional[str]:
        token = self.get_token()
        if not token:
            console.print("[red]No token found. Run 'quanuxctl vcs setup github' first.[/red]")
            return None

        headers = {
            "Authorization": f"token {token}",
            "Accept": "application/vnd.github.v3+json"
        }
        data = {
            "name": name,
            "private": private,
            "description": "Created via QuanuX CLI"
        }
        
        try:
            with httpx.Client() as client:
                response = client.post(f"{self.api_url}/user/repos", json=data, headers=headers)
                
            if response.status_code == 201:
                repo_url = response.json().get("clone_url")
                console.print(f"[green]Successfully created GitHub repository: {name}[/green]")
                return repo_url
            else:
                console.print(f"[red]Failed to create GitHub repo: {response.text}[/red]")
                return None
        except Exception as e:
            console.print(f"[red]Connection error: {e}[/red]")
            return None

class GitLabProvider(VCSProvider):
    def __init__(self):
        super().__init__("gitlab")
        self.api_url = "https://gitlab.com/api/v4"

    def create_repo(self, name: str, private: bool) -> Optional[str]:
        token = self.get_token()
        if not token:
            console.print("[red]No token found. Run 'quanuxctl vcs setup gitlab' first.[/red]")
            return None
            
        headers = {"Private-Token": token}
        data = {
            "name": name,
            "visibility": "private" if private else "public",
            "description": "Created via QuanuX CLI"
        }
        
        try:
            with httpx.Client() as client:
                response = client.post(f"{self.api_url}/projects", json=data, headers=headers)
                
            if response.status_code == 201:
                repo_url = response.json().get("http_url_to_repo")
                console.print(f"[green]Successfully created GitLab project: {name}[/green]")
                return repo_url
            else:
                console.print(f"[red]Failed to create GitLab project: {response.text}[/red]")
                return None
        except Exception as e:
            console.print(f"[red]Connection error: {e}[/red]")
            return None

def get_provider(name: str) -> Optional[VCSProvider]:
    if name.lower() == "github":
        return GitHubProvider()
    elif name.lower() == "gitlab":
        return GitLabProvider()
    return None
