import { useRef, useMemo, useLayoutEffect } from 'react';
import { useFrame } from '@react-three/fiber';
import { Vector3, Color, QuadraticBezierCurve3 } from 'three';
import { Line, QuadraticBezierLine, Instance, Instances, Html } from '@react-three/drei';
import * as THREE from 'three';

interface DataLayersProps {
    radius?: number;
}

// Helper: Lat/Lng -> Vector3
function latLngToVector3(lat: number, lng: number, radius: number): Vector3 {
    const phi = (90 - lat) * (Math.PI / 180);
    const theta = (lng + 180) * (Math.PI / 180);
    const x = -(radius * Math.sin(phi) * Math.cos(theta));
    const z = radius * Math.sin(phi) * Math.sin(theta);
    const y = radius * Math.cos(phi);
    return new Vector3(x, y, z);
}

// Data Sets
const EXCHANGES = [
    // AMERICAS
    { name: 'NYSE (New York)', lat: 40.7128, lng: -74.0060, color: '#00ccff' },
    { name: 'B3 (Sao Paulo)', lat: -23.5505, lng: -46.6333, color: '#00ccff' },
    // EMEA
    { name: 'LSE (London)', lat: 51.5074, lng: -0.1278, color: '#00ccff' },
    { name: 'FRA (Frankfurt)', lat: 50.1109, lng: 8.6821, color: '#00ccff' }, // Deutsche Borse
    { name: 'PAR (Paris)', lat: 48.8566, lng: 2.3522, color: '#00ccff' }, // Euronext
    // APAC
    { name: 'JPX (Tokyo)', lat: 35.6762, lng: 139.6503, color: '#ff0055' },
    { name: 'SGX (Singapore)', lat: 1.3521, lng: 103.8198, color: '#00ccff' },
    { name: 'HKEX (Hong Kong)', lat: 22.3193, lng: 114.1694, color: '#00ccff' },
    { name: 'SSE (Shanghai)', lat: 31.2304, lng: 121.4737, color: '#00ccff' },
    { name: 'NSE (Mumbai)', lat: 19.0760, lng: 72.8777, color: '#00ccff' },
    { name: 'ASX (Sydney)', lat: -33.8688, lng: 151.2093, color: '#00ccff' },
];

const NODES = [
    { name: 'AWS-Virgina', lat: 39.0438, lng: -77.4874, type: 'cloud' },
    { name: 'AWS-Frankfurt', lat: 50.1109, lng: 8.6821, type: 'cloud' },
    { name: 'GCP-Tokyo', lat: 35.6895, lng: 139.6917, type: 'cloud' },
    { name: 'Azure-SouthCentral', lat: 29.4241, lng: -98.4936, type: 'cloud' },
    { name: 'QuanuX-Edge-CHI', lat: 41.8781, lng: -87.6298, type: 'edge' }, // Chicago CME
    { name: 'QuanuX-Edge-LDN', lat: 51.5074, lng: -0.1278, type: 'edge' },
];

const CONNECTIONS = [
    { from: 'NYSE (New York)', to: 'LSE (London)', latency: '29ms' },
    { from: 'NYSE (New York)', to: 'QuanuX-Edge-CHI', latency: '4ms' },
    { from: 'LSE (London)', to: 'FRA (Frankfurt)', latency: '8ms' },
    { from: 'FRA (Frankfurt)', to: 'SGX (Singapore)', latency: '140ms' },
    { from: 'JPX (Tokyo)', to: 'SGX (Singapore)', latency: '35ms' },
    { from: 'JPX (Tokyo)', to: 'HKEX (Hong Kong)', latency: '20ms' },
    { from: 'QuanuX-Edge-CHI', to: 'Azure-SouthCentral', latency: '12ms' },
    { from: 'NYSE (New York)', to: 'B3 (Sao Paulo)', latency: '60ms' },
    { from: 'SGX (Singapore)', to: 'ASX (Sydney)', latency: '90ms' },
];

export function DataLayers({ radius = 5 }: DataLayersProps) {
    return (
        <group>
            <ExchangeMarkers radius={radius} />
            <NodeMarkers radius={radius} />
            <GlobalConnections radius={radius} />
            <SatelliteSwarm radius={radius} count={1500} />
            <StarlinkUplinks radius={radius} />
        </group>
    );
}

// --- SUB COMPONENTS ---

function ExchangeMarkers({ radius }: { radius: number }) {
    return (
        <group>
            {EXCHANGES.map((ex, i) => {
                const pos = latLngToVector3(ex.lat, ex.lng, radius).multiplyScalar(1.0);
                return (
                    <group key={i} position={pos} lookAt={[0, 0, 0] as any}>
                        {/* Vertical Pillar */}
                        <mesh rotation={[Math.PI / 2, 0, 0]} position={[0, 0, 0.6]}>
                            <cylinderGeometry args={[0.03, 0.03, 1.2, 6]} />
                            <meshBasicMaterial color={ex.color} opacity={0.6} transparent />
                        </mesh>
                        {/* Label */}
                        <group position={[0, 0, 1.3]} rotation={[Math.PI / 2, Math.PI, 0]}> {/* Adjusted for billboard logic implicitly via lookAt? No, Html handles it */}
                            {/* Using simple meshes for markers, labels could be HTML but trying to keep it performance heavy 3D? Html is better */}
                        </group>
                    </group>
                );
            })}
        </group>
    )
}

function NodeMarkers({ radius }: { radius: number }) {
    return (
        <group>
            {NODES.map((node, i) => {
                const pos = latLngToVector3(node.lat, node.lng, radius).multiplyScalar(1.0);
                const color = node.type === 'edge' ? '#00ff88' : '#aa00ff'; // Green Edge, Purple Cloud
                return (
                    <mesh key={i} position={pos}>
                        <boxGeometry args={[0.12, 0.12, 0.12]} />
                        <meshStandardMaterial color={color} emissive={color} emissiveIntensity={3} toneMapped={false} />
                    </mesh>
                );
            })}
        </group>
    )
}

function GlobalConnections({ radius }: { radius: number }) {
    // Collect all points
    const pointsMap = useMemo(() => {
        const map = new Map<string, { lat: number, lng: number }>();
        [...EXCHANGES, ...NODES].forEach(p => map.set(p.name, p));
        return map;
    }, []);

    return (
        <group>
            {CONNECTIONS.map((conn, i) => {
                const start = pointsMap.get(conn.from);
                const end = pointsMap.get(conn.to);
                if (!start || !end) return null;

                const startPos = latLngToVector3(start.lat, start.lng, radius);
                const endPos = latLngToVector3(end.lat, end.lng, radius);

                // Calculate control point (midpoint projected out)
                const mid = startPos.clone().add(endPos).multiplyScalar(0.5).normalize().multiplyScalar(radius * 1.3);

                return (
                    <group key={i}>
                        <QuadraticBezierLine
                            start={startPos}
                            end={endPos}
                            mid={mid}
                            color="#ffffff"
                            lineWidth={1}
                            dashed
                            dashScale={5}
                            gapSize={3}
                            opacity={0.2}
                            transparent
                        />
                        {conn.latency && (
                            <Html position={mid} center>
                                <div style={{ color: 'white', fontSize: '0.2em', whiteSpace: 'nowrap', textShadow: '0 0 2px black' }}>
                                    {conn.latency}
                                </div>
                            </Html>
                        )}
                    </group>
                );
            })}
        </group>
    );
}

function SatelliteSwarm({ radius, count }: { radius: number, count: number }) {
    const meshRef = useRef<THREE.InstancedMesh>(null);
    const dummy = useMemo(() => new THREE.Object3D(), []);

    // Static Random Positions for now, animated rotation of the whole group
    const particles = useMemo(() => {
        const temp = [];
        for (let i = 0; i < count; i++) {
            const r = radius * (1.15 + Math.random() * 0.2); // Altitude 1.15x - 1.35x
            const theta = Math.random() * Math.PI * 2;
            const phi = Math.acos(2 * Math.random() - 1);

            const x = r * Math.sin(phi) * Math.cos(theta);
            const y = r * Math.sin(phi) * Math.sin(theta);
            const z = r * Math.cos(phi);
            temp.push({ x, y, z });
        }
        return temp;
    }, [count, radius]);

    useLayoutEffect(() => {
        if (!meshRef.current) return;
        particles.forEach((p, i) => {
            dummy.position.set(p.x, p.y, p.z);
            dummy.lookAt(0, 0, 0);
            dummy.scale.setScalar(Math.random() > 0.9 ? 1.5 : 0.6);
            dummy.updateMatrix();
            meshRef.current?.setMatrixAt(i, dummy.matrix);
        });
        meshRef.current.instanceMatrix.needsUpdate = true;
    }, [particles, dummy]);

    useFrame((state) => {
        if (!meshRef.current) return;
        meshRef.current.rotation.y -= 0.0002; // Orbit
    });

    return (
        <instancedMesh ref={meshRef} args={[undefined, undefined, count]}>
            <dodecahedronGeometry args={[0.02, 0]} />
            <meshBasicMaterial color="#ffcc00" transparent opacity={0.6} />
        </instancedMesh>
    );
}

// Visualize "Uplinks" from QuanuX Nodes to nearby Satellites
function StarlinkUplinks({ radius }: { radius: number }) {
    // For each QuanuX node, find a "satellite" position directly above it at orbit altitude
    const links = useMemo(() => {
        return NODES.map(node => {
            const start = latLngToVector3(node.lat, node.lng, radius);
            // Simulate a satellite being roughly above
            const end = start.clone().multiplyScalar(1.25);
            return { start, end };
        });
    }, [radius]);

    return (
        <group>
            {links.map((link, i) => (
                <group key={i}>
                    <Line
                        points={[link.start, link.end]}
                        color="#00ff88"
                        opacity={0.4}
                        transparent
                        lineWidth={1}
                    />
                    {/* Pulsing signal packet would be nice here, but simple line for now */}
                </group>
            ))}
        </group>
    )
}
