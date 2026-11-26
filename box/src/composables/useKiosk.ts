import { ref, onMounted, type Ref } from 'vue'
import { Capacitor } from '@capacitor/core'
import type { AppKioskPlugin } from '@/types/capacitor'

export interface KioskState {
  inLockTask: boolean
}

export interface UseKioskReturn {
  state: Ref<KioskState>
  exitLockTask: () => Promise<void>
  checkLockTask: () => Promise<void>
  setupExitReceiver: () => Promise<void>
}

export function useKiosk(): UseKioskReturn {
  const state = ref<KioskState>({
    inLockTask: false
  })

  let plugin: AppKioskPlugin | null = null

  const initializePlugin = async () => {
    if (!Capacitor.isPluginAvailable('AppKiosk')) {
      console.warn('AppKiosk plugin not available')
      return
    }
    plugin = (Capacitor as any).Plugins.AppKiosk as AppKioskPlugin
  }

  const checkLockTask = async () => {
    if (!plugin) return
    try {
      const result = await plugin.isInLockTask()
      state.value.inLockTask = result.inLockTask
    } catch (e) {
      console.error('Check lock task error:', e)
    }
  }

  const exitLockTask = async () => {
    if (!plugin) return
    try {
      await plugin.exitLockTask()
      state.value.inLockTask = false
    } catch (e) {
      console.error('Exit lock task error:', e)
    }
  }

  const setupExitReceiver = async () => {
    if (!plugin) return
    try {
      await plugin.setupExitReceiver()
    } catch (e) {
      console.error('Setup exit receiver error:', e)
    }
  }

  onMounted(async () => {
    await initializePlugin()
    await checkLockTask()
  })

  return {
    state,
    exitLockTask,
    checkLockTask,
    setupExitReceiver
  }
}