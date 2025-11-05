import fs from "fs";

function rmBlockComments(s) {
  return s.replace(/\/\*[\s\S]*?\*\//g, "");
}
function rmFullLineComments(s) {
  // remove lines that are comments (leading whitespace + //)
  return s.replace(/^[ \t]*\/\/.*$/gm, "");
}
function rmDanglingCommas(s) {
  // remove trailing commas before } or ]
  return s.replace(/,\s*([}\]])/g, "$1");
}

function tryParse(raw) {
  const attempts = [
    raw,
    rmBlockComments(raw),
    rmFullLineComments(raw),
    rmDanglingCommas(raw),
    rmDanglingCommas(rmBlockComments(raw)),
    rmDanglingCommas(rmFullLineComments(raw)),
    rmDanglingCommas(rmFullLineComments(rmBlockComments(raw))),
  ];
  for (const a of attempts) {
    try { return JSON.parse(a); } catch {}
  }
  throw new Error("Unable to parse JSON/JSONC after cleanup attempts.");
}

for (const f of process.argv.slice(2)) {
  const raw = fs.readFileSync(f, "utf8");
  const j = tryParse(raw);

  j.compilerOptions ??= {};
  j.compilerOptions.baseUrl ??= ".";
  j.compilerOptions.paths ??= {};
  j.compilerOptions.paths["@/*"] ??= ["src/*"];
  j.compilerOptions.paths["@/shared/*"] = ["../shared/*"];

  fs.writeFileSync(f, JSON.stringify(j, null, 2) + "\n");
  console.log(`patched: ${f}`);
}
