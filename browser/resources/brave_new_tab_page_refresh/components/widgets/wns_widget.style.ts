/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { color } from '@brave/leo/tokens/css/variables'
import { scoped } from '../../lib/scoped_css'

export const style = scoped.css`
  & {
    position: relative;

    display: flex;
    flex-direction: column;
    gap: 18px;

    width: 100%;
    min-width: 260px;

    padding: 20px;

    color: ${color.text.primary};

    border-radius: 20px;

    background: ${color.material.thin};

    backdrop-filter: blur(40px) saturate(1.5);

    border: 1px solid rgba(255, 255, 255, 0.08);

    box-shadow:
      0 8px 32px rgba(0, 0, 0, 0.14);

    animation:
      fadeIn 0.35s ease-out,
      linear widget-scroll-fade both;

    animation-timeline:
      auto,
      --ntp-main-view-timeline;

    animation-range:
      normal,
      exit-crossing 10% exit-crossing 100%;
  }

  .wns-header {
    display: flex;
    align-items: center;
    gap: 12px;
  }

  .wns-header svg {
    flex-shrink: 0;

    filter:
      drop-shadow(0 0 8px rgba(255, 120, 0, 0.25));
  }

  .header-text {
    display: flex;
    flex-direction: column;
    gap: 2px;
  }

  .header-text span {
    font-size: 15px;
    font-weight: 700;
    letter-spacing: 0.4px;
  }

  .header-text p {
    margin: 0;

    font-size: 12px;
    line-height: 1.4;

    opacity: 0.7;
  }

  .wns-actions {
    display: grid;

    grid-template-columns: repeat(2, minmax(0, 1fr));

    gap: 10px;
  }

  button {
    min-width: 0;
    width: 100%;

    border: none;
    border-radius: 12px;

    padding: 12px 14px;

    font-size: 13px;
    font-weight: 600;

    cursor: pointer;

    color: ${color.text.primary};

    background:
      rgba(255, 255, 255, 0.08);

    border:
      1px solid rgba(255, 255, 255, 0.06);

    transition:
      background 0.2s ease,
      transform 0.2s ease,
      box-shadow 0.2s ease;
  }

  button:hover {
    background:
      rgba(255, 255, 255, 0.16);

    transform: translateY(-1px);

    box-shadow:
      0 4px 12px rgba(0, 0, 0, 0.12);
  }

  button:active {
    transform: translateY(0);
  }

  @keyframes fadeIn {
    from {
      opacity: 0;
      transform: translateY(8px);
    }

    to {
      opacity: 1;
      transform: translateY(0);
    }
  }

  @keyframes widget-scroll-fade {
    from {
      opacity: 1;
    }

    to {
      opacity: 0;
    }
  }

  @media (max-width: 768px) {
    & {
      padding: 16px;
      min-width: unset;
    }

    .wns-actions {
      grid-template-columns: 1fr;
    }

    button {
      font-size: 12px;
      padding: 11px 12px;
    }
  }
`