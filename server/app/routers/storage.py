from fastapi import APIRouter, HTTPException, Body
from pydantic import BaseModel
from ..database import get_db

router = APIRouter(prefix="/api/storage", tags=["storage"])

class AttachRequest(BaseModel):
    path: str
    alias: str

class DetachRequest(BaseModel):
    alias: str

@router.post("/attach")
async def attach_database(req: AttachRequest):
    db_instance = get_db()
    try:
        # ATTACH 'path' AS alias
        # Sanitize alias? DuckDB handles it, but valid identifier needed.
        sql = f"ATTACH '{req.path}' AS {req.alias}"
        db_instance.con.query(sql)
        return {"status": "success", "message": f"Attached {req.path} as {req.alias}"}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@router.post("/detach")
async def detach_database(req: DetachRequest):
    db_instance = get_db()
    try:
        sql = f"DETACH {req.alias}"
        db_instance.con.query(sql)
        return {"status": "success", "message": f"Detached {req.alias}"}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))
