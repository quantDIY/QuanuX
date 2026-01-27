import { type ClassValue, clsx } from "clsx";
import { twMerge } from "tailwind-merge";

/** Merge conditional classnames the shadcn way */
export function cn(...inputs: ClassValue[]) {
    return twMerge(clsx(inputs));
}
