const assert = require('node:assert/strict')
const { chromium } = require('playwright')

async function main() {
  const baseUrl = process.env.PQC_WEB_URL || 'http://127.0.0.1:5176/'
  const browser = await chromium.launch({
    headless: true,
    executablePath: process.env.PQC_CHROMIUM_PATH || undefined
  })
  const page = await browser.newPage({ viewport: { width: 1440, height: 1000 } })

  await page.goto(`${baseUrl}#lab`, { waitUntil: 'networkidle' })
  await page.getByRole('heading', { name: 'X bit flip' }).waitFor()
  assert.equal(await page.locator('.demo-list button').count(), 28)

  await page.getByLabel('Category').selectOption('Topology comparison')
  await page.getByText('Same circuit: line', { exact: true }).click()
  await page.getByRole('button', { name: 'Run analyze' }).click()
  await page.getByText('CX routing records').waitFor()
  assert.match(await page.locator('.console pre').innerText(), /total_cx_equivalents=15/)

  await page.locator('.card-title select').selectOption('shortcut4')
  await page.getByRole('button', { name: 'Run analyze' }).click()
  await page.getByText('CX routing records').waitFor()
  assert.match(await page.locator('.console pre').innerText(), /total_cx_equivalents=3/)

  await page.getByRole('button', { name: 'graph.c' }).click()
  await page.locator('.source-panel').waitFor()
  assert.match(await page.locator('.source-panel pre').innerText(), /graph_shortest_path/)

  await page.goto(`${baseUrl}#slides`, { waitUntil: 'networkidle' })
  assert.match(await page.locator('.slide').innerText(), /DS LAB MINI PROJECT/i)
  assert.doesNotMatch(await page.locator('.slide').innerText(), /C11/)
  await page.getByRole('button', { name: 'Notes' }).click()
  await page.getByText('Presenter notes').waitFor()

  await browser.close()
  console.log('Web Demo Lab and presentation checks passed.')
}

main().catch(error => {
  console.error(error)
  process.exit(1)
})
