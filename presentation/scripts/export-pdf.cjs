const fs = require('node:fs/promises')
const path = require('node:path')
const { chromium } = require('playwright')
const { PDFDocument } = require('pdf-lib')

async function main() {
  const presentationDirectory = path.resolve(__dirname, '..')
  const outputPath = path.resolve(presentationDirectory, 'final-presentation.pdf')
  const url = process.env.PQC_PRESENTATION_URL || 'http://127.0.0.1:5173/#slides'
  await fs.mkdir(path.dirname(outputPath), { recursive: true })

  const browser = await chromium.launch({
    headless: true,
    executablePath: process.env.PQC_CHROMIUM_PATH || undefined
  })
  const page = await browser.newPage({ viewport: { width: 1600, height: 900 } })
  await page.goto(url, { waitUntil: 'networkidle' })
  await page.evaluate(() => document.fonts.ready)
  await page.addStyleTag({ content: `
    .view-switch, .slide nav { display: none !important; }
    .deck { width: 1600px !important; height: 900px !important; min-height: 0 !important; padding: 0 !important; }
    .slide { width: 1600px !important; height: 900px !important; max-height: none !important; border-radius: 0 !important; box-shadow: none !important; }
  ` })

  const marker = await page.locator('.slide header span').last().innerText()
  const slideCount = Number(marker.split('/').pop().trim())
  if (!Number.isInteger(slideCount) || slideCount < 1) {
    throw new Error(`Could not determine slide count from: ${marker}`)
  }

  const merged = await PDFDocument.create()
  for (let slide = 0; slide < slideCount; slide += 1) {
    const bytes = await page.pdf({
      width: '1600px', height: '900px', printBackground: true,
      margin: { top: '0', right: '0', bottom: '0', left: '0' }
    })
    const single = await PDFDocument.load(bytes)
    const [copied] = await merged.copyPages(single, [0])
    merged.addPage(copied)
    if (slide + 1 < slideCount) {
      await page.keyboard.press('ArrowRight')
      await page.waitForTimeout(60)
    }
  }

  await fs.writeFile(outputPath, await merged.save())
  await browser.close()
  console.log(`Exported ${slideCount} slides to ${outputPath}`)
}

main().catch(error => {
  console.error(error)
  process.exitCode = 1
})
