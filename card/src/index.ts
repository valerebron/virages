// Main entry point for card-tools library

// Library styles (includes Tailwind utilities)
import './style.lib.css'

// Components
export { default as Card } from './components/Card.vue'
export { default as CardElement } from './components/CardElement.vue'
export { default as Platine } from './components/Platine.vue'
export { default as Rank } from './components/Rank.vue'
export { default as PlayButton } from './components/UI/PlayButton.vue'
export { default as Loader } from './components/UI/Loader.vue'
export { default as PlayPauseIcon } from './components/UI/PlayPauseIcon.vue'

// SVG Components
export { default as HeartSVG } from './components/svg/HeartSVG.vue'
export { default as DiamondSVG } from './components/svg/DiamondSVG.vue'
export { default as ClubSVG } from './components/svg/ClubSVG.vue'
export { default as SpadeSVG } from './components/svg/SpadeSVG.vue'

// Types
export type {
  CardData,
  CardUnicode,
  CardSize,
  CardTransitionConfig,
  PlatineProgressConfig,
  PlatinePhysicsConfig,
  PlatineAnimationConfig,
} from './types/types'

// Composables
export { useAutoplayGate } from './composables/useAutoplayGate'
export { useScreenWakeLock } from './composables/useScreenWakeLock'
export { useIsTouchInterface } from './composables/useIsTouchInterface'

// Utils
export { timeout } from './utils/timeout'

// Platine utils (advanced usage)
export { default as Disc } from './utils/platine/disc'
export { default as Sampler } from './utils/platine/sampler'

// Version
export const version = '__VERSION__'