import http from 'node:http'
import { execFile } from 'node:child_process'
import { mkdtemp, readFile, rm, writeFile } from 'node:fs/promises'
import os from 'node:os'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const presentationDirectory = path.dirname(fileURLToPath(import.meta.url))
const repositoryRoot = path.resolve(presentationDirectory, '..')
const manifestPath = path.join(repositoryRoot, 'demos', 'manifest.json')
const executablePath = path.join(repositoryRoot,
  process.platform === 'win32' ? 'qmlc.exe' : 'qmlc')
const host = '127.0.0.1'
const port = 8787
const maximumBodyBytes = 2 * 1024 * 1024
const maximumInputBytes = 1024 * 1024
const allowedSources = new Set([
  'a.c', 'common.c', 'circuit.c', 'input.c', 'graph.c',
  'analysis.c', 'sim.c', 'qml.c'
])
const allowedQmlModes = new Set([
  'encoding-only', 'initialized', 'trained', 'zero', 'train'
])

function sendJson(response, status, value) {
  response.writeHead(status, {
    'Content-Type': 'application/json; charset=utf-8',
    'Access-Control-Allow-Origin': 'http://127.0.0.1:5173',
    'Access-Control-Allow-Headers': 'Content-Type',
    'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
    'Cache-Control': 'no-store'
  })
  response.end(JSON.stringify(value))
}

async function readBody(request) {
  const chunks = []
  let size = 0
  for await (const chunk of request) {
    size += chunk.length
    if (size > maximumBodyBytes) {
      throw new Error('Request body exceeds the 2 MiB bridge limit')
    }
    chunks.push(chunk)
  }
  return JSON.parse(Buffer.concat(chunks).toString('utf8'))
}

function runExecutable(argumentsList) {
  const started = Date.now()
  return new Promise((resolve) => {
    execFile(executablePath, argumentsList, {
      cwd: repositoryRoot,
      encoding: 'utf8',
      timeout: 15000,
      maxBuffer: 4 * 1024 * 1024,
      windowsHide: true,
      shell: false
    }, (error, stdout, stderr) => {
      resolve({
        stdout,
        stderr,
        exitCode: error && Number.isInteger(error.code) ? error.code
          : error ? null : 0,
        signal: error?.signal ?? null,
        timedOut: Boolean(error?.killed),
        durationMs: Date.now() - started,
        launchError: error && !Number.isInteger(error.code) ? error.message : null
      })
    })
  })
}

function safeDemoPath(relativePath) {
  if (typeof relativePath !== 'string' || !relativePath.startsWith('demos/')) {
    throw new Error('Manifest contains an invalid demo path')
  }
  const resolved = path.resolve(repositoryRoot, relativePath)
  const demoRoot = path.resolve(repositoryRoot, 'demos') + path.sep
  if (!resolved.startsWith(demoRoot)) {
    throw new Error('Demo path leaves the demos directory')
  }
  return resolved
}

async function loadDemos() {
  const manifest = JSON.parse(await readFile(manifestPath, 'utf8'))
  const topologyById = new Map(manifest.topologies.map(item => [item.id, item]))
  const demos = await Promise.all(manifest.demos.map(async (demo) => {
    const circuitText = demo.circuit
      ? await readFile(safeDemoPath(demo.circuit), 'utf8')
      : ''
    const topology = demo.topology ? topologyById.get(demo.topology) : null
    const topologyText = topology
      ? await readFile(safeDemoPath(topology.file), 'utf8')
      : ''
    return { ...demo, circuitText, topologyText }
  }))
  const topologies = await Promise.all(manifest.topologies.map(async (item) => ({
    ...item,
    text: await readFile(safeDemoPath(item.file), 'utf8')
  })))
  return { version: manifest.version, demos, topologies }
}

async function runRequest(body) {
  const operation = body.operation
  if (!['simulate', 'analyze', 'qml'].includes(operation)) {
    throw new Error('Unsupported operation')
  }
  if (operation === 'qml') {
    if (!allowedQmlModes.has(body.qmlMode)) {
      throw new Error('Unsupported QML mode')
    }
    return runExecutable(['--qml', body.qmlMode])
  }
  if (typeof body.circuitText !== 'string' ||
      Buffer.byteLength(body.circuitText) > maximumInputBytes) {
    throw new Error('Circuit text is missing or exceeds 1 MiB')
  }
  if (operation === 'analyze' && (typeof body.topologyText !== 'string' ||
      Buffer.byteLength(body.topologyText) > maximumInputBytes)) {
    throw new Error('Topology text is missing or exceeds 1 MiB')
  }

  const temporaryDirectory = await mkdtemp(path.join(os.tmpdir(), 'pqc-demo-'))
  try {
    const circuitFile = path.join(temporaryDirectory, 'input.circuit')
    await writeFile(circuitFile, body.circuitText, 'utf8')
    if (operation === 'simulate') {
      return await runExecutable(['--simulate', circuitFile])
    }
    const topologyFile = path.join(temporaryDirectory, 'input.graph')
    await writeFile(topologyFile, body.topologyText, 'utf8')
    return await runExecutable([
      '--analyze', circuitFile, '--topology', topologyFile
    ])
  } finally {
    await rm(temporaryDirectory, { recursive: true, force: true })
  }
}

const server = http.createServer(async (request, response) => {
  try {
    const url = new URL(request.url, `http://${host}:${port}`)
    if (request.method === 'OPTIONS') {
      sendJson(response, 204, {})
    } else if (request.method === 'GET' && url.pathname === '/api/health') {
      sendJson(response, 200, { status: 'ready', executable: executablePath })
    } else if (request.method === 'GET' && url.pathname === '/api/demos') {
      sendJson(response, 200, await loadDemos())
    } else if (request.method === 'GET' && url.pathname === '/api/source') {
      const source = url.searchParams.get('path')
      if (!allowedSources.has(source)) {
        sendJson(response, 400, { error: 'Source path is not allowed' })
        return
      }
      sendJson(response, 200, {
        path: source,
        githubUrl: `https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/${source}`,
        text: await readFile(path.join(repositoryRoot, source), 'utf8')
      })
    } else if (request.method === 'POST' && url.pathname === '/api/run') {
      sendJson(response, 200, await runRequest(await readBody(request)))
    } else {
      sendJson(response, 404, { error: 'Unknown bridge endpoint' })
    }
  } catch (error) {
    sendJson(response, 400, { error: error.message })
  }
})

server.listen(port, host, () => {
  console.log(`PQC Demo Lab bridge listening on http://${host}:${port}`)
  console.log(`C executable: ${executablePath}`)
})
