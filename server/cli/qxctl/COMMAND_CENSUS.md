# QuanuX Command Census & Constructor Ledger

This file represents the deterministic machine-auditable census proving mapping integration exclusively over `runtime.App` constructor bounds natively.

## Command Matrix (Viper-Free Assembly)

| Command Tree | Constructor Name | Injected Dependencies Requirements | Output Pipeline | Provider Fakes Testing |
|---|---|---|---|---|
| `dashboard` | `NewDashboardCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes (`FakeJetStream`) |
| `enable` | `NewEnableCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `engine` | `NewEngineCmd(app)` | `app.Out`, `app.NATS`, `app.Ctx` | `output.Print` | Yes (`FakeJetStream`) |
| `habitat` | `NewHabitatCmd(app)` | `app.Out`, `app.Ctx` | `output.EmitRaw` | Yes |
| `indicators` | `NewIndicatorsCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `infra` | `NewInfraCmd(app)` | `app.Out`, `app.Vault`, `app.Ctx` | `output.Print` | Yes (`FakeSecretStore`) |
| `install` | `NewInstallCmd(app)` | `app.Out`, `app.Ctx` | `output.Log` | Yes |
| `lifecycle` | `NewLifecycleCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `module` | `NewModuleCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `nest` | `NewNestCmd(app)` | `app.Out`, `app.Ctx` | `output.EmitRawf` | Yes |
| `node` | `NewNodeCmd(app)` | `app.Out`, `app.NATS`, `app.Ctx` | `output.Print` | Yes (`FakeJetStream`) |
| `orchestra` | `NewOrchestraCmd(app)` | `app.Out`, `app.Ctx` | `output.EmitRaw` | Yes |
| `predeploy` | `NewPredeployCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `probe` | `NewProbeCmd(app)` | `app.Out`, `app.Ctx` | `output.Log` | Yes |
| `query` | `NewQueryCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `remove` | `NewRemoveCmd(app)` | `app.Out`, `app.Ctx` | `output.Log` | Yes |
| `secrets` | `NewSecretsCmd(app)` | `app.Out`, `app.Vault`, `app.Ctx` | `output.Print` | Yes (`FakeSecretStore`) |
| `skills` | `NewSkillsCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `spreader` | `NewSpreaderCmd(app)` | `app.Out`, `app.Ctx` | `output.EmitRaw` | Yes |
| `storage` | `NewStorageCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `topstepx` | `NewTopstepxCmd(app)` | `app.Out`, `app.Ctx` | `output.Print` | Yes |
| `version` | `NewVersionCmd(app)` | `app.Out` | `output.Print` | Yes |

* 0 Commands map Legacy `init()`.
* 0 Commands mutate Global `var App`.
* 100% test coverage evaluated on Constructor Paths dynamically.
