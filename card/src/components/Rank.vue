<template>
  <div
    :class="[
      'rank flex items-center justify-center absolute',
      { 'suit-dark': !isRedCard, 'suit-red': isRedCard, 'revert': isDarkBackground }
    ]"
    :style="rankStyle"
  >
    <slot name="suit" :isRedCard="isRedCard" :svgComponent="svgComponent">
      <div class="suit absolute">
        <component :is="svgComponent" :class="{ 'suit-red': isRedCard, 'suit-black': !isRedCard }" />
      </div>
    </slot>
    <slot name="number" :rank="rankValue">
      <div class="number text-white font-bold absolute -mt-1" :style="numberStyle">
        {{ rankValue }}
      </div>
    </slot>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import type { CardData, CardUnicode } from '@/types/types'
import HeartSVG from '@/components/svg/HeartSVG.vue'
import DiamondSVG from '@/components/svg/DiamondSVG.vue'
import ClubSVG from '@/components/svg/ClubSVG.vue'
import SpadeSVG from '@/components/svg/SpadeSVG.vue'
import { decode } from 'blurhash'

// Unicode ranges for playing cards (U+1F0A0–U+1F0FF)
// U+1F0A0 = 🂠 (back of card)
// U+1F0A1 = 🂡 (Ace of Spades) ... U+1F0AE = 🂮 (King of Spades)
// U+1F0B1 = 🂱 (Ace of Hearts) ... U+1F0BE = 🂮 (King of Hearts)
// U+1F0C1 = 🃁 (Ace of Diamonds) ... U+1F0CE = 🃎 (King of Diamonds)
// U+1F0D1 = 🃑 (Ace of Clubs) ... U+1F0DE = 🃞 (King of Clubs)

const SUIT_MAP: Record<number, { symbol: '♠' | '♥' | '♦' | '♣'; name: string; isRed: boolean; svg: typeof HeartSVG | typeof DiamondSVG | typeof ClubSVG | typeof SpadeSVG }> = {
  0x1F0A0: { symbol: '♠', name: 'Spades', isRed: false, svg: SpadeSVG },      // 🂠 - back of card, treat as spades
  0x1F0A1: { symbol: '♠', name: 'Spades', isRed: false, svg: SpadeSVG },      // 🂡 - Ace of Spades
  0x1F0B1: { symbol: '♥', name: 'Hearts', isRed: true, svg: HeartSVG },        // 🂱 - Ace of Hearts
  0x1F0C1: { symbol: '♦', name: 'Diamonds', isRed: true, svg: DiamondSVG },     // 🃁 - Ace of Diamonds
  0x1F0D1: { symbol: '♣', name: 'Clubs', isRed: false, svg: ClubSVG },          // 🃑 - Ace of Clubs
}

const RANK_MAP: Record<number, string> = {
  1: 'A',   // Ace
  2: '2', 3: '3', 4: '4', 5: '5', 6: '6', 7: '7', 8: '8', 9: '9', 10: '10',
  11: 'V',  // Valet (Jack)
  12: 'D',  // Dame (Queen)
  13: 'R',  // Roi (King)
  14: '*',  // Joker
}

function parseCardUnicode(unicode: CardUnicode | undefined): { suit: '♠' | '♥' | '♦' | '♣' | null; rank: string | null; isRed: boolean; svgComponent: typeof HeartSVG | typeof DiamondSVG | typeof ClubSVG | typeof SpadeSVG | null } {
  if (!unicode) return { suit: null, rank: null, isRed: false, svgComponent: null }

  const codePoint = unicode.codePointAt(0)
  if (!codePoint) return { suit: null, rank: null, isRed: false, svgComponent: null }

  // Determine suit from the block
  // U+1F0A0–U+1F0AE: Spades (🂠–🂮)
  // U+1F0B1–U+1F0BE: Hearts (🂱–🂾)
  // U+1F0C1–U+1F0CE: Diamonds (🃁–🃎)
  // U+1F0D1–U+1F0DE: Clubs (🃑–🃞)

  let suitInfo = SUIT_MAP[0x1F0A0] // default

  if (codePoint >= 0x1F0A1 && codePoint <= 0x1F0AE) {
    suitInfo = SUIT_MAP[0x1F0A1]
  } else if (codePoint >= 0x1F0B1 && codePoint <= 0x1F0BE) {
    suitInfo = SUIT_MAP[0x1F0B1]
  } else if (codePoint >= 0x1F0C1 && codePoint <= 0x1F0CE) {
    suitInfo = SUIT_MAP[0x1F0C1]
  } else if (codePoint >= 0x1F0D1 && codePoint <= 0x1F0DE) {
    suitInfo = SUIT_MAP[0x1F0D1]
  } else if (codePoint === 0x1F0A0 || codePoint === 0x1F0BF || codePoint === 0x1F0CF || codePoint === 0x1F0DF) {
    // Jokers / back of card - default to spades
    suitInfo = SUIT_MAP[0x1F0A0]
  }

  // Determine rank within suit (1-14 for A,2-10,V,D,R,*)
  let rankOffset = 0
  if (codePoint >= 0x1F0A1 && codePoint <= 0x1F0AE) {
    rankOffset = codePoint - 0x1F0A1 + 1
  } else if (codePoint >= 0x1F0B1 && codePoint <= 0x1F0BE) {
    rankOffset = codePoint - 0x1F0B1 + 1
  } else if (codePoint >= 0x1F0C1 && codePoint <= 0x1F0CE) {
    rankOffset = codePoint - 0x1F0C1 + 1
  } else if (codePoint >= 0x1F0D1 && codePoint <= 0x1F0DE) {
    rankOffset = codePoint - 0x1F0D1 + 1
  }

  const rank = RANK_MAP[rankOffset] || null

  return {
    suit: suitInfo.symbol,
    rank,
    isRed: suitInfo.isRed,
    svgComponent: suitInfo.svg
  }
}

const props = withDefaults(defineProps<{
  /** Card data */
  card?: CardData;
  /** Custom position: { top, left, size } */
  position?: { top?: string; left?: string; size?: string };
  /** Custom number style */
  numberStyle?: { size?: string; lineHeight?: string };
  /** Dark background luminance threshold (0-255) */
  darkThreshold?: number;
}>(), {
  position: () => ({ top: '8%', left: '12%', size: '20%' }),
  numberStyle: () => ({ size: '38cqw', lineHeight: '38cqw' }),
  darkThreshold: 100
})

const parsedUnicode = computed(() => {
  return parseCardUnicode(props.card?.unicode)
})

const isRedCard = computed(() => parsedUnicode.value.isRed)

const svgComponent = computed(() => parsedUnicode.value.svgComponent)

const rankValue = computed(() => parsedUnicode.value.rank)

const isDarkBackground = computed(() => {
  const blurhash = props.card?.blurhash

  if (!blurhash) return false

  const pixels = decode(blurhash, 1, 1)

  const r = pixels[0]
  const g = pixels[1]
  const b = pixels[2]

  const luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b

  return luminance < props.darkThreshold
})

const rankStyle = computed(() => ({
  width: props.position?.size || '20%',
  height: props.position?.size || '20%',
  top: props.position?.top || '8%',
  left: props.position?.left || '12%',
  fontSize: '100%'
}))

const numberStyle = computed(() => ({
  width: '20%',
  fontSize: props.numberStyle?.size || '34cqw',
  lineHeight: props.numberStyle?.lineHeight || '40cqw'
}))
</script>

<style scoped>
.rank {
  transition: opacity 0.5s ease-in-out;
  container-type: inline-size;
  z-index: 10;
}

.suit {
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
}

:deep(svg) {
  width: 100%;
  height: 100%;
  display: block;
}

.number {
  width: 100%;
  font-size: 34cqw;
  line-height: 40cqw;
}

.suit-dark {
  color: black;
}

.suit-red {
  @apply text-red-600;
}
</style>