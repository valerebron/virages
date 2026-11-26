<template>
  <div class="screen-centered p-4">
    <Card :card />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import Card from './components/Card.vue'
import type { CardData, CardUnicode } from '@/types/types'

import DefaultCardMp3 from '@/assets/default.mp3?url'
import DefaultCardSvg from '@/assets/default.svg?url'

const card = ref<CardData>({
  fileUrl: DefaultCardMp3,
  coverUrl: DefaultCardSvg,
  color: 'grey',
  artist: 'artist',
  title: 'title',
  unicode: '🂠' as CardUnicode,
})
</script>
