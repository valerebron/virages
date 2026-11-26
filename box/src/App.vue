<template>
  <div class="screen-centered p-4">
    <Card :card="card" />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { Card, CardData } from 'card-tools'
import DefaultCardMp3 from '../../card/src/assets/default.mp3?url'
import DefaultCardSvg from '../../card/src/assets/default.svg?url'

const card = ref<CardData>({
  fileUrl: DefaultCardMp3,
  coverUrl: DefaultCardSvg,
  color: '#6b7280',
  artist: 'Test Artist',
  title: 'Test Title',
  unicode: '🂠',
})
</script>

<style>
@import 'card-tools/style.css';
</style>
