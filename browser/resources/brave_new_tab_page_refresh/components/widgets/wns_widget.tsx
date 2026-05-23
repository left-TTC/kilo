import * as React from 'react'
import { style } from './wns_widget.style'

export function WnsWidget() {
    function openDnsSite() {
        window.open('https://dns.rust', '_blank')
    }

    function openDnsGate() {
        window.open('chrome://settings/web3', '_blank')
    }

    function openKiloResource() {
        window.open('https://github.com/left-TTC/kilo-browser.git', '_blank')
    }

    function openKiloGuide() {
        window.open('https://dns.rust', '_blank')
    }

    return (
        <div data-css-scope={style.scope}>
            <div className='wns-header'>
                <svg
                    width='20'
                    height='20'
                    viewBox='0 0 24 24'
                    fill='none'
                    stroke='currentColor'
                    strokeWidth='2'
                    strokeLinecap='round'
                    strokeLinejoin='round'
                >
                    <circle cx='6' cy='12' r='2' />
                    <circle cx='18' cy='6' r='2' />
                    <circle cx='18' cy='18' r='2' />
                    <line x1='8' y1='12' x2='16' y2='6' />
                    <line x1='8' y1='12' x2='16' y2='18' />
                </svg>

                <div className='header-text'>
                    <span>WNS</span>
                    <p>Web Name Service tools and shortcuts</p>
                </div>
            </div>

            <div className='wns-actions'>
                <button onClick={openDnsSite}>
                    Go DNS Site
                </button>

                <button onClick={openDnsGate}>
                    Manage DNS Gate
                </button>

                <button onClick={openKiloResource}>
                    Kilo Browser Resources
                </button>

                <button onClick={openKiloGuide}>
                    Learn Kilo
                </button>
            </div>
        </div>
    )
}