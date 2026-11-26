<template>
  <article :role="props.role" :class="[
    'card-element cursor-pointer',
    isFaceUp ? 'face-up' : 'face-down'
  ]" :tabindex="props.tabindex" @click="onCardClick" @keydown.enter="onCardEnter">
    <div class="flip-inner" ref="cardElement">
      <!-- Face-Up -->
      <main
        class="face-up z-10 hover:shadow-xl hover:scale-110 transition-all shadow-lg flex flex-col overflow-hidden border-2">
        <slot name="rank">
          <Rank :card="card" />
        </slot>

        <!-- Cover -->
        <figure class="cover relative">
          <slot name="cover" :isLoading="isLoading" :isPlaying="isPlaying">
            <PlayButton :is-loading="isLoading" :is-playing="isPlaying" interactive :tabindex="-1"
                aria-label="play" class="z-10" />
            <img :src="props.card.coverUrl" alt="card cover"
              class="cover-image relative z-0 w-full h-full object-cover object-center" draggable="false" loading="lazy" />
          </slot>
        </figure>
      </main>

      <!-- Face-Down -->
      <footer
        class="face-down z-10 bg-opacity-10 bg-white shadow-lg flex flex-col overflow-hidden select-none">
        <figure class="cover">
          <slot name="face-down" :isLoading="isLoading" :isPlaying="isPlaying">
            <PlayButton :is-loading="isLoading" :is-playing="isPlaying" />
            <img :src="FaceDownSvg" class="cover-image" :style="{ backgroundColor: `color-mix(in srgb, ${card.color}, black 70%)` }" draggable="false" />
          </slot>
        </figure>
      </footer>
    </div>
  </article>
</template>

<script setup lang="ts">
import type { CardData } from '@/types/types'
import { computed } from 'vue'
import Rank from './Rank.vue'
import PlayButton from './UI/PlayButton.vue'

// Import SVG assets for library distribution
import FaceDownSvg from '@/assets/face-down.svg?inline'
import DefaultCardSvg from '@/assets/default.svg?inline'

const emit = defineEmits<{
  click: [event: MouseEvent | KeyboardEvent]
  'button-click': [payload: { button: string; card: CardData }]
}>()

const props = withDefaults(defineProps<{
  card: CardData;
  isFaceUp?: boolean;
  isLoading?: boolean;
  isPlaying?: boolean;
  role?: string; // ARIA role
  tabindex?: string | number;
}>(), {
  isFaceUp: false,
  isPlaying: false,
  role: 'button',
  tabindex: '0'
})

const onCardClick = (event: MouseEvent) => {
  emit('click', event)
}

const onCardEnter = (event: KeyboardEvent) => {
  emit('click', event)
}
</script>

<style lang="scss">
.card-element {
  perspective: 1000px;
  @apply transition-all scale-100 size-full;

  .flip-inner {
    position: relative;
    width: 100%;
    height: 100%;
    transition: transform 0.6s;
    transform-style: preserve-3d;
    transform-origin: center;
  }

  .face-down,
  .face-up {
    position: absolute;
    width: 100%;
    height: 100%;
    backface-visibility: hidden;
    will-change: transform;
    background-color: white;
    border: 1px #80808020 solid;
    padding: 2.4vmin;
    .open &  {
      padding: 4vmin;
    }
    border-radius: 4vmin;
    .cover {
      border-radius: 2vmin;
      border: 1px #80808020 solid;
      @apply flex-1 flex justify-center items-center overflow-hidden cursor-pointer;
    }
  }

  .face-up {
    transform: rotateY(0deg);
    transition: box-shadow 0.6s;
  }

  .face-down {
    img {
      width: 100%;
    }

    figure {
      overflow: hidden;
    }
  }

  .face-down {
    transform: rotateY(-180deg);
  }

  &.face-down .flip-inner {
    transform: rotateY(180deg);
  }

  &.face-up .flip-inner {
    transform: rotateY(0deg);
  }

  &.face-down:hover {
    .play-button {
      opacity: 1;
    }

    .flip-inner {
      transform: rotateY(170deg);
    }
  }

  &.face-down:active {
    .play-button {
      @apply scale-90;
    }
  }

  &:focus:not(.is-disabled),
  &.current-card:not(.is-disabled) {
    @apply z-50 scale-110;
    outline: none;

    .face-up {
      @apply shadow-2xl;
      transition:
        box-shadow 0.6s,
        transform 6s;
    }
  }

  &:focus:not(.is-disabled),
  &.current-card:not(.is-disabled) {
    .play-button {
      @apply opacity-100;
    }
  }

  .face-up {

    &:hover {

      .play-button,
      .action-buttons {
        opacity: 1;
      }

      .action-buttons {
        pointer-events: auto;
      }
    }

    .flip-inner {
      transform: rotateY(-170deg);
    }
  }


  .play-button {
    @apply absolute opacity-100;
  }

  @media (hover: hover) and (pointer: fine) {
    .play-button {
      @apply opacity-0 hover:opacity-100;
    }
  }

  .play-button {

    &.loading {
      opacity: 1 !important;
    }

  }
}
</style>