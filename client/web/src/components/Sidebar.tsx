import React from 'react';
import {
    LayoutDashboard,
    Workflow,
    Network,
    Briefcase,
    Database,
    LineChart,
    Settings,
    User,
    LogOut
} from 'lucide-react';
import { Button } from '@quanux/shared/components/ui/button';
import { Separator } from '@quanux/shared/components/ui/separator';
import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger
} from '@quanux/shared/components/ui/tooltip';
import { cn } from '@quanux/shared/lib/utils';

interface SidebarProps {
    className?: string;
    onNavigate?: (view: string) => void;
    currentView?: string;
}

interface SidebarItem {
    id: string;
    label: string;
    icon: React.ElementType;
}

const mainItems: SidebarItem[] = [
    { id: 'dashboard', label: 'Dashboard', icon: LayoutDashboard },
    { id: 'strategy', label: 'Strategy Builder', icon: Workflow },
    { id: 'backtests', label: 'Backtests', icon: LineChart },
    { id: 'integrations', label: 'Integrations', icon: Network },
    { id: 'brokers', label: 'Brokers', icon: Briefcase },
    { id: 'data', label: 'Data', icon: Database },
];

export const Sidebar: React.FC<SidebarProps> = ({ className, onNavigate, currentView = 'dashboard' }) => {
    return (
        <TooltipProvider delayDuration={0}>
            <div className={cn(
                "flex flex-col h-full w-[60px] bg-muted/40 border-r border-border items-center py-4 gap-4",
                className
            )}>
                {/* Top Section - App Navigation */}
                <div className="flex flex-col gap-2 w-full px-2">
                    {mainItems.map((item) => (
                        <Tooltip key={item.id}>
                            <TooltipTrigger asChild>
                                <Button
                                    variant={currentView === item.id ? "secondary" : "ghost"}
                                    size="icon"
                                    className={cn(
                                        "w-full aspect-square rounded-lg",
                                        currentView === item.id && "bg-muted text-foreground ring-1 ring-border shadow-sm"
                                    )}
                                    onClick={() => onNavigate?.(item.id)}
                                >
                                    <item.icon className="w-5 h-5" />
                                    <span className="sr-only">{item.label}</span>
                                </Button>
                            </TooltipTrigger>
                            <TooltipContent side="right">
                                <p>{item.label}</p>
                            </TooltipContent>
                        </Tooltip>
                    ))}
                </div>

                <div className="px-2 w-full mt-auto">
                    <Separator className="bg-border/60" />
                </div>

                {/* Bottom Section - User & Settings */}
                <div className="flex flex-col gap-2 w-full px-2">
                    <Tooltip>
                        <TooltipTrigger asChild>
                            <Button variant="ghost" size="icon" className="w-full aspect-square rounded-lg">
                                <Settings className="w-5 h-5" />
                                <span className="sr-only">Settings</span>
                            </Button>
                        </TooltipTrigger>
                        <TooltipContent side="right">
                            <p>Settings</p>
                        </TooltipContent>
                    </Tooltip>

                    <Tooltip>
                        <TooltipTrigger asChild>
                            <Button variant="ghost" size="icon" className="w-full aspect-square rounded-lg">
                                <User className="w-5 h-5" />
                                <span className="sr-only">Profile</span>
                            </Button>
                        </TooltipTrigger>
                        <TooltipContent side="right">
                            <p>Profile</p>
                        </TooltipContent>
                    </Tooltip>
                </div>
            </div>
        </TooltipProvider>
    );
};
