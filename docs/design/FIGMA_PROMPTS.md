# QuanuX Design System Prompts (IBKR Desktop Inspired)

These prompts are designed to be fed into an AI Designer (like Claude for Figma) to generate a high-fidelity, professional trading platform UI.

## 🎨 Phase 1: Foundation & Aesthetics
**Goal**: Establish a "Premium, Dark-Mode First" design system that rivals IBKR Desktop's modern look.

> **Prompt**:
> "Create a comprehensive Design System for 'QuanuX', a high-performance institutional trading platform. 
> allow for a 'Sleek, Dark Mode' aesthetic similar to the new IBKR Desktop but more modern.
> 
> **1. Colors**:
> - **Palette**: Tone down harsh colors. Use various shades of Blue, Purple, Green, White, and multiple Greys.
> - **Background**: Deep rich gradients (hombres) utilizing dark blues and greys.
> - **Accents**: 
>   - Selection: Soft Blue or Purple gradients.
>   - Up/Profit: Professional, calm Greens.
>   - Down/Loss: Soft shades of muted Rose or Berry; avoid harsh/alarming Oranges/Reds.
> - **Text**: Hierarchy using various shades of White and Grey.
>
> **2. Typography**:
> - Font: 'Inter' or 'Roboto Mono' for numbers.
> - Hierarchy: Dense, data-heavy layout. Use 11px/12px for data grids, 14px for labels.
>
> **3. Structure**:
> - Layout: Bento Box / Modular Grid system.
> - Spacing: Compact (4px/8px grid). Minimize whitespace to maximize data density.
> - **Borders**: Avoid hard lines. Use shading, depth, and box shadows to imply borders and separation.

---

## 🧭 Phase 2: Navigation & Shell
**Goal**: Replicate the efficient Sidebar navigation of IBKR Desktop.

> **Prompt**:
> "Design the application shell for QuanuX Desktop.
> 
> **1. Left Navigation Bar (Collapsed)**:
> - Width: 60px.
> - Icons (Thin, Modern): 'Home/Dashboard', 'Quote Monitor', 'Chart', 'Option Chain', 'Scanner', 'Portfolio', 'Settings'.
> - State: Selected icon has a Cyan glow strip on the left. Hover effects are subtle fill lightens.
> 
> **2. Top Command Bar**:
> - Height: 40px.
> - Content:
>   - Global Symbol Search (CMD+K style) in the center. Large, prominently placed.
>   - Connection Status (Green Dot 'Connected to Rithmic').
>   - Account Selector Dropdown ('Account: U1234567').
>   - System Clock (UTC and Local).
>
> **3. Workspace Tabs**:
> - Below the Top Bar, allow for 'Workspace Tabs' (e.g., 'Morning Scan', 'Execution', 'Options')."

---

## 📈 Phase 3: The Core Trading Dashboard
**Goal**: A highly customizable, widget-based layout.

> **Prompt**:
> "Design the 'Execution Workspace' using a modular grid layout with the following window management rules:
> - **Window Management**: Each window/widget must be independently resizable on all views/screens.
> - **Customization**: The DOM should support unlimited customization, sizing, and easy addition of new widgets.
>
> It should contain the following independent panels (Widgets):
>
> **1. Chart Widget (Center-Left, Large)**:
> - Style: TradingView-like aesthetics. Candlesticks (Green/Red).
> - Overlays: VWAP, EMA lines.
> - Header: Symbol, Timeframe selector, Chart Type.
> - Background: Transparent/Dark.
>
> **2. Order Entry Ticket (Right Sidebar)**:
> - Header: Buy / Sell toggle switches (Big Buttons).
> - Inputs: Quantity (Stepper), Price (Limit), Type (LMT/MKT/STP).
> - 'Time in Force' Dropdown (DAY, GTC, IOC).
> - Action Button: Large 'SUBMIT ORDER' button (Green for Buy, Red for Sell).
> - **Bid/Ask Spread**: Visual bar showing current spread depth above the button.
>
> **3. Level 2 / DOM (Depth of Market)**:
> - Vertical Ladder style.
> - Center column: Price.
> - Left column: Buy Orders (Histogram).
> - Right column: Sell Orders (Histogram).
> - Highlight the 'Last Traded Price' row."

---

## ⛓️ Phase 4: Option Lattice / Chain
**Goal**: A specialized view for derivatives, inspired by IBKR's Lattice.

> **Prompt**:
> "Design an 'Option Strategy Lab' view.
>
> **1. The Chain Grid**:
> - Central Axis: Strike Prices (Vertical list).
> - Left Side: Calls (Bid/Ask/Vol/Delta).
> - Right Side: Puts (Bid/Ask/Vol/Delta).
> - Hover: Highlighting a Strike row illuminates both Call and Put sides.
>
> **2. Strategy Builder (Bottom Panel)**:
> - 'Payoff Diagram': A line graph showing Profit/Loss vs Price at expiration.
> - Leg List: 'Buy 1 CALL @ 150', 'Sell 1 CALL @ 155'.
> - Stats: Max Profit, Max Loss, Breakeven points.
>
> **3. Visual Cues**:
> - In-the-money (ITM) rows have a slightly darker, tinted background (dark blue/purple).
> - Greeks (Delta, Gamma, Theta) shown in muted colors."

---

## 💎 Phase 5: Interaction & Polish
**Goal**: Make it feel alive.

> **Prompt**:
> "Add interaction states and 'Pro' features:
> 
> **1. Panel Linking (Color Groups)**:
> - Add a small 'Link Icon' (Chain) in the header of every panel (Chart, DOM, Order Entry).
> - Show a color dot (Blue, Red, Group 1, Group 2). Linking two panels to 'Blue' syncs their Symbol.
>
> **2. Data Streaming Effects**:
> - When a price updates in the Quote Monitor, flash the cell background briefly:
>   - Green flash if Price Up.
>   - Red flash if Price Down.
> - This flash should be milliseconds long and fade out (CSS transition).
>
> **3. Micro-interactions**:
> - Toggle Switches: Smooth slide animations.
> - Dropdowns: Backdrop blur (Glassmorphism) when opened."

---

## 🏠 Phase 6: The Quant Command Center (Landing Page)
**Goal**: A unified "Mission Control" for all distributed strategies and accounts.

> **Prompt**:
> "Design the 'Command Center' (Home) dashboard. This is the first screen the user sees.
> 
> **1. Layout Overview (3-Column)**:
> - **Left Column**: 'Active Strategies' (Strategies with open positions).
> - **Center Column**: 'Consolidated Performance' (The main stage).
> - **Right Column**: 'Idle / Scanning Strategies' (Strategies running but flat).
> 
> **2. Center Stage (Performance & Control)**:
> - **Top Header**: Huge 'Total Daily PnL' (e.g., '+$2,450.00') with percentage.
> - **Key Metrics Row**: 'Net Liquidity', 'Buying Power Used' (Margin Gauge), 'System Latency' (Health).
> - **Main Chart**: A large Intraday Equity Curve (Area chart, Green gradient fill).
> - **Drill-down Tabs**: 'All Accounts', 'Broker: IBKR', 'Broker: Rithmic'.
> - **Global Controls**: (Moved to 'active strategies' column).
> - **Bottom Section**: 'Trade Logs & Confirms'. A data grid showing recent fills (Time, Symbol, Side, Price, Strategy).
> 
> **3. Left Column (Active Strategies)**:
> - **Header**: Includes 'Emergency Toggle' (Hidden by default).
> - **Action**: Toggling ON reveals a single **'Flatten & Pause'** button next to it (exits all strategies simultaneously).
> - Card-based list of strategies with CURRENT risk.
> - Each Card:
>   - Name (e.g., 'MeanRev_SPY').
>   - PnL (Green/Red badge).
>   - Net Position (e.g., '+5 ES').
>   - **Mini-Controls**: 'Flatten', 'Pause' (Clock icon for 'Pause for X hours').
> 
> **4. Right Column (The Waiting Room)**:
> - **Header**: Include a toggle to hide 'Heartbeat' stats and any nested children (collapsible).
> - List of strategies that are 'Hunting/Scanning'.
> - Visuals: Pulse animations or status dots (Yellow = Scanning, Green = Signal Pending).
> - **Status Indicators**: Show last heartbeat time (placeholder feature).

---

## ⚡ Phase 7: Performance & Responsiveness (UX)
**Goal**: The UI must feel "Instantaneous" (C++ Native Speed), zero latency perception.

> **Prompt**:
> "Design for 'Instant Response' and 'Native-Feel' performance:
>
> **1. Optimistic UI Patterns**:
> - Visual feedback (clicks, toggles) must happen **immediately (<16ms)** on interaction, before server confirmation.
> - Use specific styles for 'Pending State' (e.g., slightly desaturated or pulsing opacity) vs 'Confirmed State' (solid).
>
> **2. Zero Layout Shift (via Persistence)**:
> - **Dynamic Saved Views**: Grids and widgets are dynamic and user-resizable.
> - **Persistence as Source of Truth**: The initial render MUST load the user's *exact* last-saved dimensions/layout from local storage immediately, avoiding any "pop-in" or resizing after load.
> - **Skeleton Matching**: Skeleton loaders must match the *saved* dimensions, not a generic default.
>
> **3. Transition Physics**:
> - **GPU Layers**: All animations (modals, slide-overs) must use `transform` and `opacity` only.
> - **Snap Transitions**: View switching (e.g., Tab A -> Tab B) should be instant. Avoid long fade-ins; prefer 'Snap' or extremely fast (100ms) slide-overs.
>
> **4. Data Velocity**:
> - Design for high-frequency updates (60fps data ticking).
> - Use monospaced fonts for numbers to prevent jittering width changes."

---

## 🖥️ Phase 8: Advanced Workspace & Power Tools
**Goal**: Enable "Power User" freedom. Unlimited customization, multi-monitor support, and direct system access.

> **Prompt**:
> "Design a 'Free-Form Workspace' manager:
>
> **1. Infinite Canvas / Docker**:
> - **The Grid**: Allow users to drag-and-drop an **unlimited** number of widgets (e.g., '20 Charts', '10 DOMs') onto the workspace.
> - **Detachable Windows**: Every panel must have a 'Pop-out' icon. Clicking it detaches the widget into a separate OS-level window for multi-monitor setups.
> - **Layout Engine**: Support Tiling (snap-to-grid) AND Floating (overlap) modes.
>
> **2. The CLI Terminal Interface**:
> - **Access**: Add a prominent '>_' (Terminal) icon to the main Navigation Bar.
> - **The Widget**: A floating, resizable 'QuanuX Console' window.
>   - **Visuals**: Retro-modern monospace font (JetBrains Mono/Fira Code), semi-transparent black background (`#000000CC`), glowing green cursor.
>   - **Function**: This connects directly to `quanuxctl`. It renders text streams, ASCII tables, and interactive prompts.
>   - **Header**: Simple controls: 'Clear', 'Detach', 'Opacity Slider'."

---

## 📊 Phase 9: Quote Monitor 2.0 & Market Intelligence
**Goal**: Transform the simple quote list into a "Market Situation Room".

> **Prompt**:
> "Redesign the 'Quote Monitor' widget with advanced data density and discovery tools:
>
> **1. The Grid (Multi-Widget Container)**:
> - **Structure**: The Quote Monitor is NOT a single grid. It is a **flexible workspace (Masonry/Dock)** that contains **multiple, independent Quote Widgets**.
> - **Widget Logic**: Users can have 5 different "Quote Boxes" on screen at once (e.g., 'Tech Watchlist', 'Energy Spreads', 'Pre-Market Movers').
> - **Independence**: Each Quote Widget is independently resizable and has its own column configuration (e.g., specific columns for different asset classes).
> - **Visuals**: Condensed rows (11px font), alternating row colors (zebra striping).
>
> **2. Smart Tabs & Curated Presets**:
> - **The '+' Button**: Clicking '+' on the tab bar does NOT just open a blank tab. It opens a **'Preset Menu'**.
> - **Presets List**: 'Heatmap', 'Sectors', 'FX', 'Agriculture', 'Precious Metals', 'Energy', 'Bonds', 'Rates', 'Equities', 'International'.
>
> **3. Sector Heatmap View**:
> - **Layout**: When 'Heatmap' or 'Sectors' is selected, render a **Flexbox/Grid Container**.
> - **Group Logic**: Group tiles by Sector (e.g., 'Technology', 'Energy').
> - **Interaction**:
>   - **Hover**: Highlighting a sector group dims the others.
>   - **Click**: Opens a new **'Drill-down Tab'** for that specific sector, revealing a massive list of constituent tickers.
>
> **4. Futures & Commodity Logic**:
> - **Ordering Rules**: For commodity tabs (e.g., 'Energy', 'Ag'), use 'Term Structure' logic:
>   1.  **Front Month** (Most active contract).
>   2.  **Common Spreads** (e.g., Calendar Spreads).
>   3.  **Back Months** (Out-months).
>   - **Dynamic Grouping**: Automatically group these related contracts together."

---

## 🌍 Phase 10: Visual DevOps & Infrastructure Globe
**Goal**: A stunning, 3D animated globe visualizing the QuanuX distributed infrastructure, global market activity, and connectivity. Inspired by high-end cyber-threat maps but for financial infrastructure.

> **Prompt**:
> "Design a 'Visual DevOps Globe' widget. This is a 3D interactive WebGL visualization.
>
> **1. The Globe Object (Point Cloud & Wireframe)**:
> - **Material**: **CRITICAL**: The globe must NOT contain any solid surfaces. It is a **Volumetric Point Cloud** or **Wireframe**.
>   - **Oceans**: **Empty Negative Space**. Do NOT render a surface for water. The oceans are defined only by the *absence* of land dots and the faint structural grid.
>   - **Geography (CRITICAL)**: The floating dots must be a **projection of Earth's actual topology**. Do not use random noise. The viewer MUST be able to clearly recognize specific landmasses (e.g., Florida, Japan, UK).
>   - **Landmasses**: Formed entirely by **floating, glowing cyan/white dots** (Data Points). No solid terrain.
>   - **Borders (Essential)**: Use fainter, thin glowing lines to trace major **Country Borders** within the dot clouds. This structure is REQUIRED to define the geography.
>   - **Night Side**: The 'Land Dots' dim slightly, while major cities bloom with brighter, vertical light pillars.
>   - **Structure**: A faint, geodesic wireframe (rhombus pattern, 5% opacity) encapsulates the cloud to give it spherical volume.
> - **Atmosphere**: A digital 'Horizon Ring' (thin glowing line) separating the globe from space.
> - **Rotation**: Auto-rotates slowly. User can click-drag to spin faster or stop.
> - **Axis Control**: **CRITICAL**: The globe must allow free movement up and down the North/South axis (tilting the poles toward the viewer).
> - **Zoom**: Scroll to zoom in from orbit down to detailed node view.
>
> **2. Data Layers (Visualized Elements)**:
> - **Major Exchanges (Blue)**: Pulsing markers for NYSE, NASDAQ, LSE, Tokyo, HK, CME (Chicago).
>   - *Label*: Hover shows 'CME - Open' or 'Closed'.
> - **QuanuX Nodes (Green)**: Bright, distinct markers for our deployed strategies/nodes.
> - **Data Centers (Purple)**: Official broker/exchange colocation centers (e.g., NJ, Chicago, London).
> - **Satellites (SpaceX)**: Tiny moving orbits/dots representing low-latency satellite links (Starlink).
>   - *Detail*: Faint lines connecting satellites to ground stations.
> - **Connectivity Lines**: Arcs moving between Nodes and Exchanges.
>   - *Animation*: 'Packets' (light pulses) traveling along the arcs. Speed represents latency.
>
> **3. Interactive Tools & Sidebar**:
> - **Icon**: A stylized 'Globe' icon in the main sidebar to access this view.
> - **Control Panel (Overlay)**:
>   - 'Play/Pause' rotation.
>   - 'Time Scrubber': Slider to roll time backward/forward (shifting the day/night terminator line).
>   - 'Layer Toggles': Checkboxes to hide/show Satellites, Exchanges, or Latency Lines.
>
> **4. Polishing Details**:
> - **Reflections**: Subtle specular highlights on the oceans.
> - **Background**: Deep space black with faint stars (not distracting).
> - **Performance**: Mention 'Three.js' or 'WebGL' optimization in the design notes so the dev knows it's a heavy graphic element."
