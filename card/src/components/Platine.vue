<template>
  <div class="platine" :class="{ 'loading': isLoadingTrack, 'mounted': isMounted, 'playing': isPlaying }">
    <div v-if="false" class="debug">
      <slot name="debug-overlay" :discProgress="debugDiscProgress" :audioProgress="debugAudioProgress" :speed="currentSpeed" :isReversed="isReversed">
        <button @click="Reverse">
          <b v-if="isReversed">reversed</b>
          <b v-else>normal</b>
        </button>
        <button @click="Rewind">
          Rewind
        </button>
        <button @click="NextFourSec">
          Next Five
        </button>
        <button @click="PrevFourSec">
          Prev Five
        </button>
        <div class="debug__track">
          <div class="debug__bar">
            <div class="debug__bar-fill debug__bar-fill--disc" :style="{ width: `${debugDiscProgress}%` }"></div>
            <div class="debug__bar-fill debug__bar-fill--audio" :style="{ width: `${debugAudioProgress}%` }"></div>
          </div>
          <div class="debug__labels">
            <span>disc {{ formatDebugTime(debugDiscSeconds) }}</span>
            <span>audio {{ formatDebugTime(debugAudioSeconds) }}</span>
            <span>speed {{ currentSpeed.toFixed(2) }}</span>
          </div>
        </div>
      </slot>
    </div>
    <div class="disc" ref="discRef" id="disc">
      <div class="bobine" :style="{
        height: visibleProgressPercentage + '%',
        width: visibleProgressPercentage + '%'
      }"></div>
      <button class="power-button" :class="{ isPowerActive }" @click="Power" @touchstart.passive="isPowerActive = true"
        @touchend="isPowerActive = false" @touchcancel="isPowerActive = false" :disabled="isLoadingTrack">
        <slot name="disc-center" :isLoading="isLoadingTrack">
          <img class="macaron" :src="resolvedCenterIcon" v-if="!isLoadingTrack">
        </slot>
      </button>
      <div class="turn-point" v-if="!isLoadingTrack">
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import type { CardData, PlatineAnimationConfig, PlatinePhysicsConfig, PlatineProgressConfig } from '@/types/types'
import { ref, onMounted, onUnmounted, watch, computed } from 'vue'
import Disc from '@/utils/platine/disc'
import Sampler from '@/utils/platine/sampler'
import { useScreenWakeLock } from '@/composables/useScreenWakeLock'

// Import SVG assets for library distribution
import LogoSvg from '@/assets/logo.svg?inline'

const props = withDefaults(defineProps<{
  /** Card data */
  card?: CardData;
  /** Indique si cette platine est la carte actuellement ouverte/visible */
  active?: boolean;
  delegateReplayOnEnd?: boolean;
  /** Custom center icon for the disc */
  centerIcon?: string;
  /** Custom end-of-track icon */
  endIcon?: string;
  /** Progress range configuration */
  progressConfig?: PlatineProgressConfig;
  /** Animation duration configuration */
  animationConfig?: PlatineAnimationConfig;
  /** Physics configuration for scratch/inertia */
  physicsConfig?: PlatinePhysicsConfig;
  /** Seek interval in seconds for keyboard navigation */
  seekInterval?: number;
  /** Custom icons for disc center */
  icons?: {
    center?: string;
    end?: string;
  };
}>(), {
  card: undefined,
  active: true,
  delegateReplayOnEnd: false,
  centerIcon: undefined,
  endIcon: undefined,
  progressConfig: () => ({ minPercentage: 17, maxPercentage: 100 }),
  animationConfig: () => ({
    accelerationDuration: 1500,
    decelerationDuration: 1500,
    rewindSlowDownDuration: 300,
    rewindScratchDuration: 200,
    seekScratchDuration: 300,
    seekInterval: 4
  }),
  physicsConfig: () => ({
    inertiaFriction: 0.9,
    inertiaAmplification: 12,
    maxScratchSpeed: 3.5
  }),
  seekInterval: 4
})

const emit = defineEmits(['loaded', 'finishPlaying', 'play-state-changed', 'replay-requested'])

// Merge defaults with provided config
const progressConfig = computed(() => ({
  ...props.progressConfig,
  minPercentage: 17,
  maxPercentage: 100
}))

const animationConfig = computed(() => ({
  ...props.animationConfig,
  accelerationDuration: 1500,
  decelerationDuration: 1500,
  rewindSlowDownDuration: 300,
  rewindScratchDuration: 200,
  seekScratchDuration: 300,
  seekInterval: 4
}))

const physicsConfig = computed(() => ({
  ...props.physicsConfig,
  inertiaFriction: 0.9,
  inertiaAmplification: 12,
  maxScratchSpeed: 3.5
}))

// Resolved icons (props > icons object > defaults)
const resolvedCenterIcon = computed(() => {
  return props.icons?.center || props.centerIcon || LogoSvg
})

// State
const isLoadingTrack = ref(false)
const isFirstPlay = ref(true)
const isEndOfTrack = ref(false)
const isPowerActive = ref(false)

const progressPercentage = ref(0)
const frozenProgressPercentage = ref<number | null>(null)
const currentTurns = ref(0)
const totalTurns = ref(0)
const debugDiscSeconds = ref(0)
const debugAudioSeconds = ref(0)

const disc = ref<Disc | null>(null)
const sampler = ref<Sampler | null>(null)
const discRef = ref<HTMLElement>()
const isMounted = ref(false)

const isPlaying = computed(() => Math.abs(Math.round(sampler.value?.currentSpeed || 0)) !== 0)
const isReversed = computed(() => disc.value?.isReversed || false)
const currentSpeed = computed(() => sampler.value?.currentSpeed || 0)
const debugDiscProgress = computed(() => {
  const duration = getDuration()
  if (!duration) return 0
  return (debugDiscSeconds.value / duration) * 100
})
const debugAudioProgress = computed(() => {
  const duration = getDuration()
  if (!duration) return 0
  return (debugAudioSeconds.value / duration) * 100
})
const visibleProgressPercentage = computed(() => frozenProgressPercentage.value ?? progressPercentage.value)
const { requestWakeLock, releaseWakeLock } = useScreenWakeLock()

let bobineFreezeTimer: ReturnType<typeof window.setTimeout> | null = null

let isScratchEffectActive = false
let scratchGeneration = 0

const canAutoplayNow = async () => {
  if (!sampler.value) return false
  return await sampler.value.ensureAudioContextRunning()
}

const initPlatine = (element: HTMLElement) => {
  discRef.value = element
  disc.value = new Disc(element)
  sampler.value = new Sampler()

  if (disc.value) {
    disc.value.callbacks.onStop = () => {
      sampler.value?.pause()
    }

    disc.value.callbacks.onDragStart = async () => {
      if (sampler.value && disc.value) {
        const canPlay = await sampler.value.ensureAudioContextRunning()
        if (!canPlay) return

        sampler.value.play(disc.value.secondsPlayed || 0, false)
      }
    }

    disc.value.callbacks.onDragEnded = (secondsPlayed) => {
      if (!sampler.value || !disc.value) return

      sampler.value.play(Math.max(0, secondsPlayed), false)
      updateTurns()
    }

    disc.value.callbacks.onLoop = ({ playbackSpeed, isReversed, secondsPlayed }) => {
      if (!isScratchEffectActive && Math.abs((sampler.value?.currentSpeed || 0) - playbackSpeed) > 0.01) {
        const safeSecondsPlayed = Math.max(0, secondsPlayed)
        sampler.value?.updateSpeed(playbackSpeed, isReversed, safeSecondsPlayed)
      }
      updateTurns()
    }
  }
}

const updateTurns = () => {
  if (!disc.value) return

  debugDiscSeconds.value = disc.value.secondsPlayed
  debugAudioSeconds.value = sampler.value?.getDebugPlayheadSeconds?.() || 0

  const newTurns = disc.value.secondsPlayed * 0.75
  const newTotalTurns = (disc.value as any)._duration * 0.75

  const minPercentage = progressConfig.value.minPercentage
  const maxPercentage = progressConfig.value.maxPercentage
  const progressRatio = disc.value.secondsPlayed / (disc.value as any)._duration
  const newProgressPercentage = minPercentage + (progressRatio * (maxPercentage - minPercentage))

  if (
    Math.abs(currentTurns.value - newTurns) > 0.01 ||
    Math.abs(totalTurns.value - newTotalTurns) > 0.01 ||
    Math.abs(progressPercentage.value - newProgressPercentage) > 0.1
  ) {
    currentTurns.value = newTurns
    totalTurns.value = newTotalTurns
    progressPercentage.value = newProgressPercentage
    isEndOfTrack.value = disc.value.secondsPlayed + 0.1 >= (disc.value as any)._duration
    if (isEndOfTrack.value) {
      emit('finishPlaying')
    }
  }
}

const loadAudio = async (card: CardData) => {
  if (!sampler.value || !card) return

  const fileUrl = (card as any).fileUrl || (card as any).url_audio
  if (!fileUrl) return

  isLoadingTrack.value = true
  try {
    await sampler.value.loadTrack(fileUrl)
    if (disc.value) {
      disc.value.setDuration(sampler.value.duration)
      disc.value.setAngle(0)
      isEndOfTrack.value = false
      currentTurns.value = 0
      totalTurns.value = sampler.value.duration * 0.75
      progressPercentage.value = progressConfig.value.minPercentage
      updateTurns()
    }
  } finally {
    isLoadingTrack.value = false
    emit('loaded')
  }
}

const play = async (position = 0, withAcceleration = true) => {
  isFirstPlay.value = false
  if (!disc.value || !sampler.value || !props.card) return false

  const canPlay = await sampler.value.ensureAudioContextRunning()
  if (!canPlay) {
    return false
  }

  if (position === 0) {
    disc.value.setAngle(0)
    isEndOfTrack.value = false
    currentTurns.value = 0
    progressPercentage.value = progressConfig.value.minPercentage
  }

  sampler.value.play(position, withAcceleration)

  if (withAcceleration) {
    disc.value.powerOnWithAcceleration()
  } else {
    disc.value.powerOn()
  }

  emit('play-state-changed', true)
  void requestWakeLock()
  return true
}

const setVolume = (volume: number) => {
  sampler.value?.setVolume(volume)
}

const getCurrentTime = () => {
  return disc.value?.secondsPlayed || 0
}

const getDuration = () => {
  return sampler.value?.duration || 0
}

const getRemainingTime = () => {
  const duration = getDuration()
  if (!duration) return Infinity
  return Math.max(0, duration - getCurrentTime())
}

const formatDebugTime = (seconds: number) => {
  if (!Number.isFinite(seconds) || seconds <= 0) {
    return '00:00.00'
  }

  const minutes = Math.floor(seconds / 60)
  const remainingSeconds = seconds - (minutes * 60)
  return `${String(minutes).padStart(2, '0')}:${remainingSeconds.toFixed(2).padStart(5, '0')}`
}

const jumpToPosition = async (position: number) => {
  if (!disc.value || !sampler.value) return false

  const duration = getDuration()
  if (!Number.isFinite(duration) || duration <= 0) {
    return false
  }

  const target = Math.max(0, Math.min(position, duration))

  sampler.value.play(target, false)
  disc.value.setAngle(target * 0.75 * Math.PI * 2)
  updateTurns()
  void requestWakeLock()
  return true
}

const jumpToNearEnd = async (secondsBeforeEnd = 5) => {
  const duration = getDuration()
  if (!Number.isFinite(duration) || duration <= 0) {
    return false
  }

  const target = Math.max(0, duration - secondsBeforeEnd)
  return await jumpToPosition(target)
}

const unload = () => {
  cleanup()
  emit('play-state-changed', false)
  void releaseWakeLock()
  isEndOfTrack.value = false
  progressPercentage.value = 0
  currentTurns.value = 0
  totalTurns.value = 0
}

const freezeBobineFor = (durationMs: number) => {
  if (bobineFreezeTimer) {
    window.clearTimeout(bobineFreezeTimer)
  }

  frozenProgressPercentage.value = progressPercentage.value
  bobineFreezeTimer = window.setTimeout(() => {
    frozenProgressPercentage.value = null
    bobineFreezeTimer = null
  }, durationMs)
}

const pause = () => {
  if (!disc.value || !sampler.value) return

  sampler.value.pauseWithDeceleration()
  disc.value.powerOffWithDeceleration()
  emit('play-state-changed', false)
  void releaseWakeLock()
}

const togglePlay = async () => {
  if (isPlaying.value) {
    pause()
    return false
  } else {
    const currentPosition = disc.value?.secondsPlayed || 0
    return await play(currentPosition)
  }
}

const cleanup = () => {
  if (disc.value) {
    disc.value.stop()
    disc.value.powerOff()
  }
  if (sampler.value) {
    sampler.value.pause()
  }
  void releaseWakeLock()
}

const Power = async () => {
  if (isEndOfTrack.value) {
    if (props.delegateReplayOnEnd) {
      emit('replay-requested')
      return
    }

    await Rewind()
    return
  }
  await togglePlay()
}

const Reverse = () => {
  if (!disc.value || !sampler.value) return

  const currentPosition = disc.value.secondsPlayed || 0
  const wasPlaying = !disc.value.isStopped()

  disc.value.reverse()
  sampler.value.reverse(wasPlaying ? currentPosition : 0)
}

const Rewind = async () => {
  if (!disc.value || !sampler.value) return

  if (disc.value.isStopped()) {
    await play(0, false)
  }

  const wasPlaying = isPlaying.value
  const currentSpeed = disc.value.playbackSpeed

  if (!wasPlaying) {
    disc.value.setAngle(0)
    updateTurns()
    return
  }

  isScratchEffectActive = true
  const generation = ++scratchGeneration

  const slowDownDuration = animationConfig.value.rewindSlowDownDuration
  const startTime = performance.now()
  const startSpeed = Math.abs(currentSpeed) || 1

  const slowDown = (timestamp: number) => {
    const elapsed = timestamp - startTime
    const progress = Math.min(elapsed / slowDownDuration, 1)

    const easeOut = 1 - Math.pow(1 - progress, 2)

    const newSpeed = startSpeed * (1 - easeOut) + 0.05
    sampler.value?.setPlaybackRate(newSpeed)

    if (progress < 1) {
      requestAnimationFrame(slowDown)
    } else {
      performRewind()
    }
  }

  const performRewind = () => {
    sampler.value?.play(0, false)
    disc.value?.setAngle(0)

    const scratchDuration = animationConfig.value.rewindScratchDuration
    const scratchStart = performance.now()

    const scratch = (timestamp: number) => {
      const elapsed = timestamp - scratchStart
      const progress = Math.min(elapsed / scratchDuration, 1)

      if (progress < 0.5) {
        const scratchProgress = progress * 2
        const scratchSpeed = 1 - (scratchProgress * 1.8)
        sampler.value?.setPlaybackRate(scratchSpeed)
      } else {
        const pullProgress = (progress - 0.5) * 2
        const pullSpeed = -0.8 + (pullProgress * 1.8)
        sampler.value?.setPlaybackRate(pullSpeed)
      }

      if (progress < 1) {
        requestAnimationFrame(scratch)
      } else {
        sampler.value?.setPlaybackRate(1)

        if (generation === scratchGeneration) {
          isScratchEffectActive = false
        }
      }
    }

    requestAnimationFrame(scratch)
  }

  requestAnimationFrame(slowDown)
}

const seek = async (direction: 'forward' | 'backward') => {
  if (!disc.value || !sampler.value) return

  const currentPosition = disc.value.secondsPlayed || 0
  const seekInterval = animationConfig.value.seekInterval
  const newPosition = direction === 'forward'
    ? Math.min(currentPosition + seekInterval, (disc.value as any)._duration)
    : Math.max(currentPosition - seekInterval, 0)
  const wasPlaying = isPlaying.value

  isScratchEffectActive = true
  const generation = ++scratchGeneration

  const scratchEffect = () => {
    const scratchDuration = animationConfig.value.seekScratchDuration
    const scratchStart = performance.now()
    const startAngle = currentPosition * 0.75 * Math.PI * 2
    const targetAngle = newPosition * 0.75 * Math.PI * 2

    const scratch = (timestamp: number) => {
      const elapsed = timestamp - scratchStart
      const progress = Math.min(elapsed / scratchDuration, 1)

      if (progress < 0.5) {
        const scratchProgress = progress * 2
        const scratchSpeed = direction === 'forward'
          ? 1 + (scratchProgress * 2)
          : 1 - (scratchProgress * 2)
        sampler.value?.setPlaybackRate(scratchSpeed)
      } else {
        const pullProgress = (progress - 0.5) * 2
        const pullSpeed = direction === 'forward'
          ? 3 - (pullProgress * 2)
          : -1 + (pullProgress * 2)
        sampler.value?.setPlaybackRate(pullSpeed)
      }

      const currentAngle = startAngle + ((targetAngle - startAngle) * progress)
      disc.value?.setAngle(currentAngle)

      if (progress < 1) {
        requestAnimationFrame(scratch)
      } else {
        if (wasPlaying) {
          sampler.value?.play(newPosition, false)
        }
        disc.value?.setAngle(targetAngle)
        if (generation === scratchGeneration) {
          isScratchEffectActive = false
        }
      }
    }

    requestAnimationFrame(scratch)
  }

  scratchEffect()
}

const NextFourSec = () => seek('forward')
const PrevFourSec = () => seek('backward')

const handleKeyDown = (e: KeyboardEvent) => {
  if (!props.active) return

  if (e.code === 'ArrowRight' || e.key === 'ArrowRight') {
    e.preventDefault()
    NextFourSec()
  } else if (e.code === 'ArrowLeft' || e.key === 'ArrowLeft') {
    e.preventDefault()
    PrevFourSec()
  }
}

onMounted(async () => {
  isMounted.value = true
  if (discRef.value) {
    initPlatine(discRef.value)
    if (props.card) {
      await loadAudio(props.card)
    }
  }

  window.addEventListener('keydown', handleKeyDown)
})

onUnmounted(() => {
  window.removeEventListener('keydown', handleKeyDown)
  isMounted.value = false
  if (bobineFreezeTimer) {
    window.clearTimeout(bobineFreezeTimer)
  }
  cleanup()
})

watch(() => props.card, (newCard) => {
  if (newCard) {
    loadAudio(newCard)
  }
})

defineExpose({
  canAutoplayNow,
  pause,
  play,
  jumpToNearEnd,
  freezeBobineFor,
  togglePlay,
  Reverse,
  Rewind,
  setVolume,
  getCurrentTime,
  getDuration,
  getRemainingTime,
  unload,
})
</script>

<style lang="scss">
.platine {
  overflow: hidden;
  position: relative;
  display: flex;
  width: 100%;
  height: 100%;

  .debug {
    position: absolute;
    z-index: 120;
    inset: 10px 10px auto 10px;
    display: grid;
    gap: 8px;
    padding: 10px 12px;
    border-radius: 12px;
    background: rgba(8, 8, 12, 0.78);
    color: #f3f4f6;
    backdrop-filter: blur(12px);
    font-size: 12px;
    line-height: 1.2;
    box-shadow: 0 12px 30px rgba(0, 0, 0, 0.28);
    pointer-events: none;
  }

  .debug button {
    pointer-events: auto;
    width: fit-content;
  }

  .debug__track {
    display: grid;
    gap: 6px;
  }

  .debug__bar {
    position: relative;
    height: 10px;
    border-radius: 999px;
    overflow: hidden;
    background: rgba(255, 255, 255, 0.12);
  }

  .debug__bar-fill {
    position: absolute;
    inset: 0 auto 0 0;
    border-radius: inherit;
  }

  .debug__bar-fill--disc {
    background: rgba(96, 165, 250, 0.55);
  }

  .debug__bar-fill--audio {
    background: rgba(251, 191, 36, 0.55);
  }

  .debug__labels {
    display: flex;
    flex-wrap: wrap;
    gap: 10px;
    justify-content: space-between;
    opacity: 0.95;
    font-variant-numeric: tabular-nums;
  }

  .disc {
    pointer-events: auto;
    position: absolute;
    background-color: transparent;
    position: relative;
    aspect-ratio: 1;
    width: 100%;
    overflow: hidden;
    border-radius: 50%;
    cursor: grab;
    background-position: center;
    background-size: cover;
    padding: 6%;

    .loading & {
      box-shadow: none;
    }
  }

  .turn-point {
    @apply absolute top-1/2 right-2 size-1/12 rounded-full bg-esyellow;
  }

  .disc.is-scratching {
    cursor: grabbing;
  }

  .power-button {
    position: absolute;
    z-index: 100;
    cursor: pointer;
    transition: all 0.1s;
    display: flex;
    justify-content: center;
    align-items: center;
    transform-origin: center;
    width: 33%;
    height: 33%;
    border-radius: 100%;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);

    &:focus-visible {
      outline: none;
      box-shadow: none;
    }

    .macaron {
      position: absolute;
      filter: grayscale(1);
      transition: transform 0.1s, filter 0.8s;
      @apply size-2/3;
    }

    &:active,
    &.isPowerActive {
      .macaron {
        transform: scale(0.8);
      }
    }
  }

  &.playing .power-button .macaron {
    filter: grayscale(0);
  }

  .disc-middle {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 20px;
    height: 20px;
    background: rgb(26, 26, 26);
    border-radius: 50%;
  }

  .bobine {
    width: 17%;
    height: 17%;
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    transition: transform 0.1s;
    @apply relative bg-black bg-opacity-70 rounded-full;
  }

  .disc:has(.power-button:active) .bobine,
  .disc:has(.power-button.isPowerActive) .bobine {
    transform: translate(-50%, -50%) scale(0.8);
  }
}
</style>