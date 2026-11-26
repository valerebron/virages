<template>
  <button :tabindex="props.tabindex" class="play-button"
    :class="{ 'is-loading': isLoading }" :disabled="isLoading" type="button"
    :style="buttonStyle">
    <slot name="icon" :isLoading="isLoading" :isPlaying="isPlaying">
      <Loader v-if="isLoading" />
      <PlayPauseIcon v-else :isPlaying="isPlaying" :style="iconStyle" />
    </slot>
  </button>
</template>

<script setup lang="ts">
import PlayPauseIcon from './PlayPauseIcon.vue'
import Loader from './Loader.vue'
import { computed } from 'vue'

const props = withDefaults(defineProps<{
  isLoading?: boolean;
  isPlaying?: boolean;
  tabindex?: number | undefined;
  /** Button color */
  color?: string;
  /** Button background color */
  bgColor?: string;
  /** Button size as percentage of container */
  size?: number;
  /** Custom icon component */
  icon?: any;
}>(), {
  isLoading: false,
  isPlaying: false,
  color: '#fdec50', // esyellow
  bgColor: 'rgba(0, 0, 0, 0.5)',
  size: 31
})

const buttonStyle = computed(() => ({
  color: props.color,
  backgroundColor: props.bgColor,
  height: `${props.size}%`,
  fontSize: `${props.size * 1.4}%`,
  borderRadius: '100%',
  aspectRatio: '1'
}))

const iconStyle = computed(() => ({
  color: props.color
}))
</script>

<style scoped lang="scss">
.play-button {
  @apply text-esyellow bg-black/50 text-4xl font-bold;
  border-radius: 100%;
  height: 30%;
  font-size: 420%;
  aspect-ratio: 1;
  display: flex;
  align-items: center;
  justify-content: center;

  &::after {
    @apply bg-esyellow;
    width: 30%;
    height: 30%;
    clip-path: polygon(0% 5%, 95% 50%, 0% 95%);
    border-radius: 10px;
  }

  .loader {
    @apply fill-esyellow opacity-70;
    padding: 22%;
  }
}
</style>
