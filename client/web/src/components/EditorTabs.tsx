import React from 'react';
import { X, FileCode } from 'lucide-react';

interface EditorTabsProps {
    openFiles: string[];
    activeFile: string | null;
    onTabSelect: (filepath: string) => void;
    onTabClose: (filepath: string) => void;
}

export const EditorTabs: React.FC<EditorTabsProps> = ({ openFiles, activeFile, onTabSelect, onTabClose }) => {
    if (openFiles.length === 0) return null;

    return (
        <div className="flex items-center overflow-x-auto border-b border-border bg-muted/30">
            {openFiles.map(filepath => (
                <div
                    key={filepath}
                    className={`
                        flex items-center gap-2 px-3 py-2 text-xs border-r border-border cursor-pointer min-w-[100px] max-w-[200px] group
                        ${activeFile === filepath ? 'bg-background text-primary border-t-2 border-t-primary' : 'bg-transparent text-muted-foreground hover:bg-muted/50'}
                    `}
                    onClick={() => onTabSelect(filepath)}
                >
                    <FileCode className="w-3 h-3 opacity-70" />
                    <span className="truncate flex-1" title={filepath}>
                        {filepath.split('/').pop()}
                    </span>
                    <button
                        className={`opacity-0 group-hover:opacity-100 hover:bg-red-500/20 hover:text-red-500 rounded p-0.5 transition-opacity ${activeFile === filepath ? 'opacity-100' : ''}`}
                        onClick={(e) => {
                            e.stopPropagation();
                            onTabClose(filepath);
                        }}
                    >
                        <X className="w-3 h-3" />
                    </button>
                </div>
            ))}
        </div>
    );
};
