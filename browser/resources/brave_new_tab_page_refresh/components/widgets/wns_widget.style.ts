/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { color } from '@brave/leo/tokens/css/variables'
import { scoped } from '../../lib/scoped_css'

export const style = scoped.css`
  & {
    position: relative;
    color: ${color.text.primary};
    border-radius: 20px;
    background: ${color.material.thin};
    backdrop-filter: blur(40px) saturate(1.5);
    border: 1px solid rgba(255, 255, 255, 0.1);
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.15);

    display: flex;
    flex-direction: column;
    padding: 20px;
    gap: 16px;
    min-width: 240px;

    animation: fadeIn 0.4s ease-out, linear widget-scroll-fade both;
    animation-timeline: auto, --ntp-main-view-timeline;
    animation-range: normal, exit-crossing 10% exit-crossing 100%;
  }

  .wns-header {
    display: flex;
    align-items: center;
    gap: 10px;
    font-size: 14px;
    font-weight: 700;
    letter-spacing: 0.5px;
    text-transform: uppercase;
    opacity: 0.9;
  }

  .wns-header svg {
    filter: drop-shadow(0 0 4px rgba(255, 80, 0, 0.3));
  }

  .wns-status {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 13px;
    font-weight: 500;
    padding: 8px 12px;
    background: rgba(255, 255, 255, 0.05);
    border-radius: 12px;
    width: fit-content;
  }

  .wns-status strong {
    display: flex;
    align-items: center;
    gap: 6px;
  }

  .wns-status strong::before {
    content: "";
    width: 8px;
    height: 8px;
    border-radius: 50%;
    display: inline-block;
  }

  /* Enabled 状态 */
  .wns-status.enabled strong {
    color: #4ade80;
  }

  .wns-status.enabled strong::before {
    background: #4ade80;
    box-shadow: 0 0 8px #4ade80;
  }

  /* Disabled 状态 */
  .wns-status.disabled strong {
    color: #fb7185;
  }

  .wns-status.disabled strong::before {
    background: #fb7185;
  }

  /* Checking 状态 */
  .wns-status.checking strong {
    color: #fbbf24;
  }

  .wns-status.checking strong::before {
    background: #fbbf24;
    animation: pulse 1.5s infinite;
  }

  .wns-actions {
    display: flex;
    gap: 10px;
  }

  button {
    flex: 1;
    border: none;
    border-radius: 10px;
    padding: 10px 14px;
    font-size: 12px;
    font-weight: 600;
    cursor: pointer;
    background: rgba(255, 255, 255, 0.1);
    color: ${color.text.primary};
    transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    border: 1px solid rgba(255, 255, 255, 0.05);
  }

  button:hover {
    background: rgba(255, 255, 255, 0.18);
    transform: translateY(-1px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
  }

  button:active {
    transform: translateY(0);
  }

  @keyframes pulse {
    0% { opacity: 1; transform: scale(1); }
    50% { opacity: 0.5; transform: scale(1.2); }
    100% { opacity: 1; transform: scale(1); }
  }

  @keyframes fadeIn {
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
  }

  @keyframes widget-scroll-fade {
    from { opacity: 1; }
    to { opacity: 0; }
  }

    h1 {
        font-size: 10px;
        font-weight: 500;
        line-height: 1.5;
        margin: 0;
        opacity: 0.75;
        max-width: 220px;
    }

    .status-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    display: inline-block;
    margin: 0 4px;
  }

  .status-dot.enabled {
    background: #4ade80;
    box-shadow: 0 0 6px rgba(74, 222, 128, 0.8);
  }

  .status-dot.disabled {
    background: #fb7185;
  }

  .status-dot.checking {
    background: #fbbf24;
    animation: pulse 1.5s infinite;
  }
`