import * as React from 'react'
import { style } from './wns_widget.style'
import { loadTimeData } from '$web-common/loadTimeData';

export function WnsWidget() {
    const wnsEnabled = loadTimeData.getBoolean("ifKilo")

    function openKiloGuide() {
        window.open("https://dns.kilo", "_blank")
    }

    function openSetGate() {
        window.open("chrome://settings/web3", "_blank");
    }

    return (
        <div data-css-scope={style.scope}>
            <div className="wns-header">
                <svg
                    width="20"
                    height="20"
                    viewBox="0 0 24 24"
                    fill="none"
                    stroke="currentColor"
                    strokeWidth="2"
                    strokeLinecap="round"
                    strokeLinejoin="round"
                >
                    <circle cx="6" cy="12" r="2"></circle>
                    <circle cx="18" cy="6" r="2"></circle>
                    <circle cx="18" cy="18" r="2"></circle>
                    <line x1="8" y1="12" x2="16" y2="6"></line>
                    <line x1="8" y1="12" x2="16" y2="18"></line>
                </svg>
                <span>WNS</span>
            </div>

            <div style={{display:'flex', flexDirection:'row', justifyContent:'space-between'}}>
                <div className="wns-status">
                    <span
                        className={`status-dot ${
                            wnsEnabled === null
                                ? "checking"
                                : wnsEnabled
                                ? "enabled"
                                : "disabled"
                        }`}
                    />
                    Status:{" "}
                    <strong>
                        {wnsEnabled === null
                            ? "Checking..."
                            : wnsEnabled
                            ? "Enabled"
                            : "Disabled"}
                    </strong>
                </div>
                <h1>
                    Enabled means Kilo domains can be resolved. If access is not possible, please adjust the gate settings.
                </h1>
            </div>

            <div className="wns-actions">

                <button onClick={openKiloGuide}>
                    Learn how to use Kilo
                </button>

                <button onClick={openSetGate}>
                    Manage dns gate
                </button>
            </div>
        </div>
    )
}