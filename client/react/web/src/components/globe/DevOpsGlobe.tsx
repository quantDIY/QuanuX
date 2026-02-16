import { useRef, useState, useEffect } from 'react';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Stars } from '@react-three/drei';
import { Suspense } from 'react';
import { EarthPointCloud } from './EarthPointCloud';
import { DataLayers } from './DataLayers';

export function DevOpsGlobe() {
    const [isRotating, setIsRotating] = useState(true);

    return (
        <div className="w-screen h-screen bg-black relative">
            <Canvas camera={{ position: [0, 0, 15], fov: 45 }}>
                <Suspense fallback={null}>
                    <ambientLight intensity={0.5} />
                    <pointLight position={[10, 10, 10]} intensity={1} />

                    <Stars radius={100} depth={50} count={5000} factor={4} saturation={0} fade speed={1} />

                    <group rotation={[0, 0, 0.4]}> {/* Tilt axis */}
                        <EarthPointCloud radius={5} count={20000} />
                        <DataLayers radius={5} />
                    </group>

                    <OrbitControls
                        enablePan={false}
                        enableZoom={true}
                        minDistance={8}
                        maxDistance={25}
                        autoRotate={isRotating}
                        autoRotateSpeed={0.5}
                    />
                </Suspense>
            </Canvas>

            {/* UI Overlay */}
            <div className="absolute bottom-8 left-1/2 -translate-x-1/2 flex flex-col items-center gap-4">
                {/* Controls */}
                <div className="flex items-center gap-2 bg-black/80 backdrop-blur-md px-4 py-2 rounded-full border border-white/10">
                    <button
                        onClick={() => setIsRotating(!isRotating)}
                        className={`px-3 py-1 rounded text-xs font-mono transition-colors ${isRotating ? 'bg-cyan-500/20 text-cyan-400 hover:bg-cyan-500/30' : 'bg-white/10 text-white hover:bg-white/20'
                            }`}
                    >
                        {isRotating ? 'PAUSE ROTATION' : 'START ROTATION'}
                    </button>
                </div>

                {/* Status Bar */}
                <div className="bg-black/60 backdrop-blur-md px-6 py-3 rounded-full border border-cyan-500/30 flex items-center gap-4">
                    <span className="text-cyan-400 font-mono text-xs animate-pulse">● LIVE DATA STREAM</span>
                    <div className="h-4 w-[1px] bg-white/20"></div>
                    <span className="text-white/60 text-xs font-mono">15 NODES ACTIVE</span>
                </div>
            </div>
        </div>
    );
}
