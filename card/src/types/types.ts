export interface CardData {
  fileUrl: string
  coverUrl?: string
  blurhash?: string
  title?: string
  artist?: string
  color?: string
  startTime?: number
  unicode?: CardUnicode
}

export type CardUnicode = '🂠' | '🂡' | '🂢' | '🂣' | '🂤' | '🂥' | '🂦' | '🂧' | '🂨' | '🂩' | '🂪' | '🂫' | '🂬' | '🂭' | '🂮' | '🂱' | '🂲' | '🂳' | '🂴' | '🂵' | '🂶' | '🂷' | '🂸' | '🂹' | '🂺' | '🂻' | '🂼' | '🂽' | '🂾' | '®' | '🃁' | '🃂' | '🃃' | '🃄' | '🃅' | '🃆' | '🃇' | '🃈' | '🃉' | '🃊' | '🃋' | '🃌' | '🃍' | '🃎' | '🃏' | '🃑' | '🃒' | '🃓' | '🃔' | '🃕' | '🃖' | '🃗' | '🃘' | '🃙' | '🃚' | '🃛' | '🃜' | '🃝' | '🃞' | '🃟' | '🃠' | '🃡' | '🃢' | '🃣' | '🃤' | '🃥' | '🃦' | '🃧' | '🃨' | '🃩' | '🃪' | '🃫' | '🃬' | '🃭' | '🃮' | '🃯' | '🃰' | '🃱' | '🃲' | '🃳' | '🃴' | '🃵'

export interface CardSize {
  width: string
  aspectRatio: string
  maxWidth?: string
  maxHeight?: string
}

export interface CardTransitionConfig {
  duration: number
  easing: string
}

export interface PlatineProgressConfig {
  minPercentage: number
  maxPercentage: number
}

export interface PlatinePhysicsConfig {
  inertiaFriction: number
  inertiaAmplification: number
  maxScratchSpeed: number
}

export interface PlatineAnimationConfig {
  accelerationDuration: number
  decelerationDuration: number
  rewindSlowDownDuration: number
  rewindScratchDuration: number
  seekScratchDuration: number
  seekInterval: number
}
