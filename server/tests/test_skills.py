import pytest
import shutil
from pathlib import Path
from server.mcp.skills import SkillLoader, Skill

# Define test paths
TEST_SKILLS_DIR = "server/tests/test_skills_data"
BASE_DIR = Path.cwd()

@pytest.fixture
def skill_loader(tmp_path):
    # Setup: Create a temporary skills directory
    skills_path = tmp_path / "skills"
    skills_path.mkdir()
    
    # Create a valid skill
    (skills_path / "valid-skill").mkdir()
    (skills_path / "valid-skill" / "SKILL.md").write_text(
        "---\nname: valid-skill\ndescription: A valid test skill.\n---\n# Content", encoding="utf-8"
    )
    
    # Create a skill with scripts
    (skills_path / "script-skill").mkdir()
    (skills_path / "script-skill" / "SKILL.md").write_text(
        "---\nname: script-skill\ndescription: Script test.\n---\n", encoding="utf-8"
    )
    (skills_path / "script-skill" / "scripts").mkdir()
    script_file = skills_path / "script-skill" / "scripts" / "hello.py"
    script_file.write_text("print('Hello Integration!')", encoding="utf-8")
    
    # Return loader initialized with temp path relative to CWD if possible, 
    # but here we pass absolute to mock the behavior
    loader = SkillLoader(str(skills_path))
    loader.base_dir = tmp_path # Override base_dir for test isolation
    return loader

def test_list_skills(skill_loader):
    skills = skill_loader.list_skills()
    assert len(skills) == 2
    names = [s.name for s in skills]
    assert "valid-skill" in names
    assert "script-skill" in names

def test_read_skill(skill_loader):
    content = skill_loader.read_skill("valid-skill")
    assert "# Content" in content

def test_read_missing_skill(skill_loader):
    content = skill_loader.read_skill("missing-skill")
    assert "Error: Skill 'missing-skill' not found" in content

@pytest.mark.asyncio
async def test_run_script(skill_loader):
    output = await skill_loader.run_script("script-skill", "hello.py")
    assert "Execution Success" in output
    assert "Hello Integration!" in output

@pytest.mark.asyncio
async def test_run_missing_script(skill_loader):
    output = await skill_loader.run_script("script-skill", "missing.py")
    assert "Error: Script 'missing.py' not found" in output
