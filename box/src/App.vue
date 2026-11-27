<template>
  <div class="screen-centered p-4">
    <Card :card="card" />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useRoute } from 'vue-router'
import { Card, CardData, CardUnicode } from 'card-tools'

const route = useRoute()
const id = route.params.id ?? 'default'

const mp3Url = new URL(`../../card/src/assets/${id}.mp3`, import.meta.url).href
const svgUrl = new URL(`../../card/src/assets/${id}.svg`, import.meta.url).href
const jpgUrl = new URL(`../../card/src/assets/${id}.jpg`, import.meta.url).href

let unicode: CardUnicode | undefined;
switch (true) {
  case id.includes('abel'):
    unicode = '🂽';
    break;
  case id.includes('amber'):
    unicode = '🃕';
    break;
  case id.includes('strokes'):
    unicode = '🃁';
    break;
}

const card = ref<CardData>({
  fileUrl: mp3Url,
  coverUrl: (id === 'default') ? svgUrl : jpgUrl,
  color: '#6b7280',
  artist: 'Artist',
  title: 'Title',
  unicode: unicode,
})

</script>

<style>
@import 'card-tools/style.css';
</style>
