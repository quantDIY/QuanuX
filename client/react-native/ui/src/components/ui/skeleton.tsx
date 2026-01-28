import * as React from 'react';
import { View } from 'react-native';
import { cn } from '../../lib/utils';

// Simple Pulse animation would be added here with Reanimated
function Skeleton({
    className,
    ...props
}: React.ComponentPropsWithoutRef<typeof View>) {
    return (
        <View
            className={cn('animate-pulse rounded-md bg-muted', className)}
            {...props}
        />
    );
}

export { Skeleton };
