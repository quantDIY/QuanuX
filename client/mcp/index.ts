import { Server } from "@modelcontextprotocol/sdk/server/index.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import {
    CallToolRequestSchema,
    ListToolsRequestSchema,
} from "@modelcontextprotocol/sdk/types.js";

// Create an MCP server
const server = new Server(
    {
        name: "QuanuX Client GUI",
        version: "0.0.1",
    },
    {
        capabilities: {
            tools: {},
        },
    }
);

// List available tools
server.setRequestHandler(ListToolsRequestSchema, async () => {
    return {
        tools: [
            {
                name: "get_client_info",
                description: "Get information about the QuanuX Client",
                inputSchema: {
                    type: "object",
                    properties: {},
                },
            },
            {
                name: "list_components",
                description: "List available UI components",
                inputSchema: {
                    type: "object",
                    properties: {},
                },
            },
        ],
    };
});

// Handle tool execution
server.setRequestHandler(CallToolRequestSchema, async (request) => {
    switch (request.params.name) {
        case "get_client_info": {
            return {
                content: [
                    {
                        type: "text",
                        text: JSON.stringify(
                            {
                                name: "QuanuX Client",
                                version: "0.0.1",
                                status: "Active",
                            },
                            null,
                            2
                        ),
                    },
                ],
            };
        }
        case "list_components": {
            return {
                content: [
                    {
                        type: "text",
                        text: JSON.stringify(
                            ["Button", "Card", "Input", "Dialog"],
                            null,
                            2
                        ),
                    },
                ],
            };
        }
        default:
            throw new Error("Unknown tool");
    }
});

async function main() {
    const transport = new StdioServerTransport();
    await server.connect(transport);
    console.error("QuanuX Client MCP Server running on stdio");
}

main().catch((error) => {
    console.error("Fatal error in main():", error);
    process.exit(1);
});
