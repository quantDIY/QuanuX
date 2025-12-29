import React, { useState } from 'react';
import { ChevronRight, ChevronDown, File, Folder } from 'lucide-react';

interface FileNode {
    name: string;
    type: 'file' | 'folder';
    children?: FileNode[];
    content?: string;
    path: string;
}

interface FileExplorerProps {
    files: Record<string, string>;
    onFileSelect: (filename: string, content: string) => void;
    selectedFile: string | null;
}

export const FileExplorer: React.FC<FileExplorerProps> = ({ files, onFileSelect, selectedFile }) => {
    // Transform flat file map to tree
    const buildTree = (files: Record<string, string>): FileNode[] => {
        const root: FileNode[] = [];

        Object.keys(files).forEach(filePath => {
            const parts = filePath.split('/');
            let currentLevel = root;

            parts.forEach((part, index) => {
                const isFile = index === parts.length - 1;
                const path = parts.slice(0, index + 1).join('/');

                let existingNode = currentLevel.find(node => node.name === part);

                if (!existingNode) {
                    const newNode: FileNode = {
                        name: part,
                        type: isFile ? 'file' : 'folder',
                        path: path,
                        children: isFile ? undefined : [],
                        content: isFile ? files[filePath] : undefined
                    };
                    currentLevel.push(newNode);
                    existingNode = newNode;
                }

                if (!isFile && existingNode.children) {
                    currentLevel = existingNode.children;
                }
            });
        });

        // Sort: Folders first, then files
        const sortNodes = (nodes: FileNode[]) => {
            nodes.sort((a, b) => {
                if (a.type === b.type) return a.name.localeCompare(b.name);
                return a.type === 'folder' ? -1 : 1;
            });
            nodes.forEach(node => {
                if (node.children) sortNodes(node.children);
            });
        };
        sortNodes(root);
        return root;
    };

    const tree = buildTree(files);

    const FileTreeItem = ({ node, depth }: { node: FileNode, depth: number }) => {
        const [isOpen, setIsOpen] = useState(true);

        const handleSelect = () => {
            if (node.type === 'file') {
                onFileSelect(node.path, node.content || '');
            } else {
                setIsOpen(!isOpen);
            }
        };

        return (
            <div>
                <div
                    className={`
                        flex items-center gap-1 py-1 px-2 cursor-pointer hover:bg-muted/50 text-sm select-none
                        ${selectedFile === node.path ? 'bg-primary/20 text-primary' : 'text-muted-foreground'}
                    `}
                    style={{ paddingLeft: `${depth * 12 + 8}px` }}
                    onClick={handleSelect}
                >
                    {node.type === 'folder' && (
                        isOpen ? <ChevronDown className="w-3 h-3" /> : <ChevronRight className="w-3 h-3" />
                    )}
                    {node.type === 'folder' ? (
                        <Folder className="w-3 h-3 text-blue-400" />
                    ) : (
                        <File className="w-3 h-3 text-gray-400" />
                    )}
                    <span>{node.name}</span>
                </div>
                {node.type === 'folder' && isOpen && node.children && (
                    <div>
                        {node.children.map(child => (
                            <FileTreeItem key={child.path} node={child} depth={depth + 1} />
                        ))}
                    </div>
                )}
            </div>
        );
    };

    return (
        <div className="h-full overflow-y-auto py-2">
            <div className="px-4 pb-2 text-xs font-semibold text-muted-foreground uppercase tracking-wider">
                Explorer
            </div>
            {tree.map(node => (
                <FileTreeItem key={node.path} node={node} depth={0} />
            ))}
        </div>
    );
};
