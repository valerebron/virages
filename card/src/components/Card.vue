<template>
  <section class="card"
    :class="{ 'open': isOpen, 'card-enter': isMounted }">
    <CardElement :card :is-face-up
      :is-loading="isLoading" :is-playing="props.isPlaying"
      @click="clickOnCard" @button-click="onButtonClick" />
    <Platine v-if="isPlatineMounted" ref="platineRef" :card :active="isOpen"
      :class="{ 'fading-out': isPlatineFading }" @loaded="onTrackLoaded" @finishPlaying="onFinishPlaying"
      v-show="isOpen || isPlatineFading" />
  </section>
</template>

<script setup lang="ts">
import type { CardData, CardUnicode } from '@/types/types'
import { timeout } from '@/utils/timeout'
import CardElement from './CardElement.vue'
import Platine from './Platine.vue'
import { useAutoplayGate } from '@/composables/useAutoplayGate'
import { useScreenWakeLock } from '@/composables/useScreenWakeLock'
import { onMounted, onBeforeUnmount, onUnmounted, ref, computed, watch } from 'vue'
import { useTemplateRef } from 'vue'

const props = withDefaults(defineProps<{
  card: CardData;
  isFaceUp?: boolean;
  isPlaying?: boolean;
  mode?: 'flip' | 'play' | 'external-player';
  autoplay?: boolean;
}>(), {
  mode: 'play'
})

const emit = defineEmits<{
  open: []
  close: []
  'play-request': [card: CardData]
  flip: [card: CardData]
  'button-click': [payload: { button: string; card: CardData }]
}>()

const isPlatineMounted = ref(false)
const isOpen = ref(false)
const isPlatineFading = ref(false)
const isMounted = ref(false)
let mountAnimationTimeout: ReturnType<typeof setTimeout> | null = null

const startMountAnimation = () => {
  if (mountAnimationTimeout) {
    clearTimeout(mountAnimationTimeout)
  }

  isMounted.value = true

  mountAnimationTimeout = setTimeout(() => {
    isMounted.value = false
    mountAnimationTimeout = null
  }, 550)
}

onMounted(async () => {
  startMountAnimation()
})

const localIsFaceUp = ref(props.isFaceUp ?? false)

watch(
  () => props.isFaceUp,
  (value) => {
    if (value !== undefined) {
      localIsFaceUp.value = value
    }
  },
  { immediate: true }
)

const isFaceUp = computed({
  get: () => localIsFaceUp.value,
  set: (value) => {
    localIsFaceUp.value = value
  }
})

const isLoading = ref(false)
const platineRef = useTemplateRef<InstanceType<typeof Platine>>('platineRef')
const { canAutoplayNow, requestAutoplayUnlock } = useAutoplayGate()
const { requestWakeLock, releaseWakeLock } = useScreenWakeLock()

const mode = computed(() => props.mode ?? 'play')
const isSelfPlayMode = computed(() => mode.value === 'play')

const revealCardIfNeeded = () => {
  if (isFaceUp.value) {
    return
  }

  isFaceUp.value = true
  emit('flip', props.card)
}

const onButtonClick = (payload: { button: string; card: CardData }) => {
  if (payload.button === 'play') {
    void clickOnCard()
  }

  emit('button-click', payload)
}

const clickOnCard = async () => {
  if (mode.value === 'external-player') {
    revealCardIfNeeded()
    emit('play-request', props.card)
    return
  }

  if (!isSelfPlayMode.value) {
    console.warn('[Card] Click in non-play mode:', mode.value, '- falling back to flip')
    revealCardIfNeeded()
    return
  }

  const canStart = await requestAutoplayUnlock()
  if (!canStart) {
    return
  }

  if (!isPlatineMounted.value) {
    isPlatineMounted.value = true
    isLoading.value = true
  } else {
    isLoading.value = true
    setTimeout(async () => {
      const started = await platineRef.value?.play?.(0)
      if (!started) {
        isLoading.value = false
        return
      }

      revealCardIfNeeded()
      announcePlaybackStarted()
      isOpen.value = true
      emit('open')
      isLoading.value = false
    }, 1200)
  }
}

const onTrackLoaded = async () => {
  revealCardIfNeeded()

  await timeout(350)
  await timeout(1200)

  const started = await platineRef.value?.play?.(0)
  if (!started) {
    isLoading.value = false
    isPlatineMounted.value = false
    return
  }

  announcePlaybackStarted()
  isOpen.value = true
  emit('open')

  isLoading.value = false
}

const onFinishPlaying = () => {
  closePlatine()
}

const closePlatine = () => {
  platineRef.value?.pause?.()

  isPlatineFading.value = true
  isOpen.value = false

  setTimeout(() => {
    isPlatineFading.value = false
    emit('close')
  }, 1200)
}

let spaceHandler: ((e: KeyboardEvent) => void) | null = null

const setupSpaceListener = () => {
  cleanupSpaceListener()
  const key = 'Space'
  spaceHandler = (e: KeyboardEvent) => {
    if (e.code === key || e.key === key || e.keyCode === 32) {
      e.preventDefault()
      platineRef.value?.togglePlay?.()
    }
  }
  window.addEventListener('keydown', spaceHandler)
}

const cleanupSpaceListener = () => {
  if (spaceHandler) {
    window.removeEventListener('keydown', spaceHandler)
    spaceHandler = null
  }
}

const announcePlaybackStarted = () => {
  void requestWakeLock()
}

const pauseOnPageHide = () => {
  platineRef.value?.pause?.()
}

const pauseOnWindowBlur = () => {
  if (document.hidden) {
    platineRef.value?.pause?.()
  }
}

watch(isOpen, (open) => {
  if (!isSelfPlayMode.value) {
    cleanupSpaceListener()
    return
  }

  if (open) {
    setupSpaceListener()
  } else {
    cleanupSpaceListener()
  }
}, { immediate: true })

watch(() => props.autoplay, async (autoplay) => {
  if (!isSelfPlayMode.value || !autoplay || isPlatineMounted.value) {
    return
  }

  const canStart = await canAutoplayNow()
  if (!canStart) {
    return
  }

  isPlatineMounted.value = true
  isLoading.value = true
}, { immediate: true })

let escapeHandler: ((e: KeyboardEvent) => void) | null = null

const setupEscapeListener = () => {
  if (escapeHandler) return
  const key = 'Escape'
  escapeHandler = (e: KeyboardEvent) => {
    if (e.key === key && isOpen.value) {
      closePlatine()
    }
  }
  window.addEventListener('keydown', escapeHandler)
}

const cleanupEscapeListener = () => {
  if (escapeHandler) {
    window.removeEventListener('keydown', escapeHandler)
    escapeHandler = null
  }
}

onMounted(() => {
  if (!isSelfPlayMode.value) {
    return
  }

  document.addEventListener('visibilitychange', pauseOnPageHide)
  window.addEventListener('pagehide', pauseOnPageHide)
  window.addEventListener('blur', pauseOnWindowBlur)

  setupEscapeListener()
})

onBeforeUnmount(() => {
  if (!isSelfPlayMode.value) {
    return
  }

  document.removeEventListener('visibilitychange', pauseOnPageHide)
  window.removeEventListener('pagehide', pauseOnPageHide)
  window.removeEventListener('blur', pauseOnWindowBlur)

  cleanupEscapeListener()
})

onUnmounted(() => {
  if (mountAnimationTimeout) {
    clearTimeout(mountAnimationTimeout)
    mountAnimationTimeout = null
  }

  cleanupSpaceListener()
  void releaseWakeLock()
})

// Expose methods for external control
defineExpose({
  play: () => clickOnCard(),
  close: closePlatine,
  isOpen: computed(() => isOpen.value),
  isLoading: computed(() => isLoading.value)
})
</script>

<style scoped lang="scss">
/* === VITESSES D'ANIMATION RÉGLABLES === */
// 1. Vitesse d'ouverture / fermeture des cartes
//    (changement de cadre, scale du disque, transition vers la platine)
$card-transition-speed: 1.2s;

// 2. Vitesse d'apparition et de disparition des cartes
//    (animation d'entrée quand les cartes arrivent dans le DOM)
//    → Doit être plus rapide que la transition ci-dessus
$card-mount-speed: 0.2s;

.card {
  position: relative;
  rotate: var(--card-rotation, 0deg);
  width: 40vmin;
  aspect-ratio: 7/10;
  max-width: 100vmin;
  max-height: 100vmin;
  transition: all var(--card-transition-duration, #{$card-transition-speed}) cubic-bezier(0.4, 0, 0.2, 1);

  &:focus-within {
    z-index: 50;
  }

  &.card-enter {
    animation: scale-in $card-mount-speed cubic-bezier(0.34, 1.56, 0.64, 1) forwards;
  }

  @keyframes scale-in {
    from {
      transform: scale(0.7);
      opacity: 0;
    }

    to {
      transform: scale(1);
      opacity: 1;
    }
  }

  @keyframes card-slide-in {
    0% {
      transform: translateY(50px) scale(0.95);
      opacity: 0;
    }

    60% {
      transform: translateY(-6px) scale(1.02);
      opacity: 1;
    }

    100% {
      transform: translateY(0) scale(1);
    }
  }

  .card-element,
  .platine {
    height: 100%;
    width: 100%;
    transition: all var(--card-transition-duration, #{$card-transition-speed}) cubic-bezier(0.4, 0, 0.2, 1);
  }

  :deep(.card-element) {
    position: absolute;

    .face-up,
    .cover {
      transition: all var(--card-transition-duration, #{$card-transition-speed}) cubic-bezier(0.4, 0, 0.2, 1);

      &:active {
        .play-button {
          @apply scale-90;
        }
      }
    }
  }

  &.open {
    rotate: 0deg;
    width: 80vmin;
    aspect-ratio: 10/10;
    transition: all var(--card-transition-duration, #{$card-transition-speed}) cubic-bezier(0.4, 0, 0.2, 1);

    .card-element {
      pointer-events: none;

      &:focus {
        @apply z-auto scale-100;
      }
    }

    &:deep(.card-element) {

      .face-up,
      .face-down {
        border-radius: 100%;
        @apply shadow-xl;

        .cover {
          border-radius: 100%;
        }

        .rank,
        .play-button {
          opacity: 0;
        }
      }
    }
  }
}

// Fade out de la platine (quand on ferme la carte)
.platine.fading-out {
  opacity: 0;
  transition: opacity var(--card-transition-duration, #{$card-transition-speed}) ease-out;
  pointer-events: none;
}
</style>