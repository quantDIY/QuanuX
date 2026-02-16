import { useRef, useState, useEffect } from 'react';
import { InstancedMesh, Object3D, Color } from 'three';

interface EarthPointCloudProps {
    radius?: number;
    count?: number; // Target number of dots (total)
}

/**
 * Visual DevOps Globe - Earth Point Cloud
 * 
 * Renders the Earth using two distinct InstancedMeshes for Land and Ocean.
 * This ensures distinct, reliable coloring and "neon" emission without vertex color complexity.
 */
export function EarthPointCloud({ radius = 5, count = 40000 }: EarthPointCloudProps) {
    const landRef = useRef<InstancedMesh>(null);
    const oceanRef = useRef<InstancedMesh>(null);
    const [isReady, setIsReady] = useState(false);

    return (
        <group>
            {/* Core Wireframe for structure */}
            <mesh>
                <sphereGeometry args={[radius * 0.98, 64, 64]} />
                <meshBasicMaterial
                    wireframe
                    color="#0a1a3a"
                    opacity={0.1}
                    transparent
                />
            </mesh>

            {/* Land Mesh - High Contrast Cyan Neon */}
            <instancedMesh ref={landRef} args={[undefined, undefined, count]}>
                <sphereGeometry args={[radius * 0.009, 6, 6]} />
                <meshStandardMaterial
                    color="#00FFFF"
                    emissive="#00FFFF"
                    emissiveIntensity={1.5}
                    toneMapped={false}
                />
            </instancedMesh>

            {/* Ocean Mesh - Faint Navy */}
            <instancedMesh ref={oceanRef} args={[undefined, undefined, count]}>
                <sphereGeometry args={[radius * 0.006, 6, 6]} />
                <meshStandardMaterial
                    color="#051040"
                    emissive="#051040"
                    emissiveIntensity={0.5}
                    transparent
                    opacity={0.4}
                />
            </instancedMesh>

            <PointCloudLogic
                landRef={landRef}
                oceanRef={oceanRef}
                radius={radius}
                count={count}
                onLoad={() => setIsReady(true)}
            />
        </group>
    );
}

function PointCloudLogic({ landRef, oceanRef, radius, count, onLoad }: {
    landRef: React.RefObject<InstancedMesh>,
    oceanRef: React.RefObject<InstancedMesh>,
    radius: number,
    count: number,
    onLoad: () => void
}) {
    useEffect(() => {
        if (!landRef.current || !oceanRef.current) return;

        console.log("EarthPointCloud: Loading texture...");
        const img = new Image();
        img.src = '/textures/earth_specular_map.png';
        img.crossOrigin = "Anonymous";

        img.onload = () => {
            console.log("EarthPointCloud: Texture loaded. Generating points...");
            const canvas = document.createElement('canvas');
            canvas.width = img.width;
            canvas.height = img.height;
            const ctx = canvas.getContext('2d');
            if (!ctx) return;

            ctx.drawImage(img, 0, 0);
            const imageData = ctx.getImageData(0, 0, img.width, img.height);
            const data = imageData.data;

            const dummy = new Object3D();
            let landCount = 0;
            let oceanCount = 0;

            for (let i = 0; i < count; i++) {
                const phi = Math.acos(-1 + (2 * i) / count);
                const theta = Math.sqrt(count * Math.PI) * phi;

                const x = radius * Math.cos(theta) * Math.sin(phi);
                const y = radius * Math.sin(theta) * Math.sin(phi);
                const z = radius * Math.cos(phi);

                // Convert 3D position to UV (0..1)
                const u = 1 - (0.5 + Math.atan2(z, x) / (2 * Math.PI)); // Flip U to fix mirror image
                const v = 0.5 - Math.asin(y / radius) / Math.PI;

                const px = Math.floor((u % 1) * img.width);
                const py = Math.floor(Math.max(0, Math.min(1, v)) * img.height);
                const index = (py * img.width + px) * 4;

                // INVERTED: Low brightness = Land (Dark Map) OR High Brightness depends on map.
                // Previous result: High Brightness was Cyan, resulted in Water being Cyan.
                // So Water is High Brightness. Land is Low.
                const brightness = data[index];
                const isLand = brightness < 50;

                dummy.position.set(x, y, z);
                dummy.lookAt(0, 0, 0);
                dummy.updateMatrix();

                if (isLand) {
                    landRef.current!.setMatrixAt(landCount++, dummy.matrix);
                } else {
                    // Keep 5% of ocean for volume
                    if (Math.random() < 0.05) {
                        oceanRef.current!.setMatrixAt(oceanCount++, dummy.matrix);
                    }
                }
            }

            // Update Counts and Buffers
            landRef.current!.count = landCount;
            oceanRef.current!.count = oceanCount;

            landRef.current!.instanceMatrix.needsUpdate = true;
            oceanRef.current!.instanceMatrix.needsUpdate = true;

            console.log(`EarthPointCloud: Generated ${landCount} Land and ${oceanCount} Ocean points.`);
            onLoad();
        };
    }, [landRef, oceanRef, radius, count]);

    return null;
}
