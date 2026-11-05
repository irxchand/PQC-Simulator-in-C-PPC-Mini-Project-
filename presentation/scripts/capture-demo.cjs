// Drives the real Demo Lab UI (needs `npm run dev` and `npm run bridge` running) and saves screenshots.
// Usage: PQC_LAB_URL=http://127.0.0.1:5173/#lab PQC_CHROMIUM_PATH=<chrome.exe> node scripts/capture-demo.cjs
const path = require('node:path')
const fs = require('node:fs/promises')
const { chromium } = require('playwright')

async function main() {
  const url = process.env.PQC_LAB_URL || 'http://127.0.0.1:5173/#lab'
  const out = path.resolve(__dirname, '..', '..', 'docs', 'demo-evidence')
  await fs.mkdir(out, { recursive: true })
  const browser = await chromium.launch({ headless: true, executablePath: process.env.PQC_CHROMIUM_PATH || undefined })
  const page = await browser.newPage({ viewport: { width: 1600, height: 1000 } })
  await page.goto(url)
  const log = []
  for (const [title, shot] of [['Same circuit: line', 'demo-1-line'], ['Same circuit: shortcut', 'demo-2-shortcut'], ['Same circuit: disconnected', 'demo-3-disconnected']]) {
    await page.locator('.demo-list button', { hasText: title }).click()
    await page.getByRole('button', { name: /^Run analyze/ }).click()
    await page.locator('.console.success, .console.warning, .console.error').waitFor({ timeout: 15000 })
    const text = await page.locator('.console pre').innerText()
    const summary = text.split('\n').find(l => l.startsWith('SUMMARY')) || '(no SUMMARY)'
    log.push(`${title}\n  ${summary}`)
    await page.screenshot({ path: path.join(out, `${shot}.png`), fullPage: true })
  }
  await page.locator('.module-links button', { hasText: 'graph.c' }).click()
  await page.locator('.source-panel pre').waitFor({ timeout: 10000 })
  const src = await page.locator('.source-panel pre').innerText()
  log.push(`graph.c source shown: ${src.length} chars, contains graph_shortest_path: ${src.includes('graph_shortest_path')}`)
  await page.screenshot({ path: path.join(out, 'demo-4-source.png'), fullPage: true })
  await page.locator('.demo-list button', { hasText: 'Trained full PQC' }).click()
  await page.getByRole('button', { name: /^Run qml/ }).click()
  await page.locator('.console.success, .console.warning, .console.error').waitFor({ timeout: 15000 })
  log.push(`PQC view: circuit svg rendered = ${await page.locator('.circuit-svg').count() === 1}, gate boxes = ${await page.locator('.circuit-svg .gate').count()}, CX = ${await page.locator('.circuit-svg .cx').count()}`)
  await page.screenshot({ path: path.join(out, 'demo-5-pqc.png'), fullPage: true })
  await page.locator('.demo-list button', { hasText: 'Same circuit: line' }).click()
  await page.getByRole('button', { name: /^Run analyze/ }).click()
  await page.locator('.route-table').waitFor({ timeout: 15000 })
  await page.evaluate(() => window.scrollTo(0, 0))
  const a = await page.locator('.visuals').boundingBox(), b = await page.locator('.route-table').boundingBox()
  await page.screenshot({ path: path.join(out, 'workbench-line.png'), fullPage: true, clip: { x: a.x, y: a.y, width: a.width, height: b.y + b.height - a.y } })
  await fs.writeFile(path.join(out, 'ui-run-log.txt'), log.join('\n') + '\n')
  console.log(log.join('\n'))
  await browser.close()
}
main().catch(e => { console.error(e); process.exitCode = 1 })
