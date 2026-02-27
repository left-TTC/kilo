// Copyright 2024 The Brave Authors
// BSD-style license.

import '//resources/cr_elements/cr_input/cr_input.js';
import '//resources/cr_elements/cr_shared_vars.css.js';
import '/shared/settings/controls/cr_policy_pref_indicator.js';

import {CrInputElement} from '//resources/cr_elements/cr_input/cr_input.js';
import {PolymerElement} from '//resources/polymer/v3_0/polymer/polymer_bundled.min.js';
import {CrPolicyPrefMixin} from '/shared/settings/controls/cr_policy_pref_mixin.js';
import {PrefControlMixin} from '/shared/settings/controls/pref_control_mixin.js';

import {getTemplate} from './settings_input.html.js';

export interface SettingsInputElement {
    $: {
        input: CrInputElement,
    };
}

const SettingsInputElementBase =
    CrPolicyPrefMixin(PrefControlMixin(PolymerElement));

export class SettingsInputElement extends SettingsInputElementBase {
    static get is() {
        return 'settings-input';
    }

    static get template() {
        return getTemplate();
    }

    static get properties() {
        return {
            label: String,
            placeholder: String,

            disabled: {
                type: Boolean,
                value: false,
                reflectToAttribute: true,
            },

            /** 区分 ipfs / rpc */
            mode: {
                type: String,
                value: 'ipfs',   // 默认 ipfs
            },

            bindingValue_: {
                type: String,
                value: '',
            },

            dropdownOpen_: {
                type: Boolean,
                value: false,
            },

            recommendedGateways_: {
                type: Array,
                value: () => [],
            },
        };
    }

    static get observers() {
        return [
            'resetInput_(pref.value)',
            'updateRecommended_(mode)',
        ];
    }

    declare bindingValue_: string;
    declare dropdownOpen_: boolean;
    declare recommendedGateways_: string[];
    declare disabled: boolean;
    declare mode: string;

    override connectedCallback() {
        super.connectedCallback();
        document.addEventListener('click', this.onOutsideClick_);
        this.updateRecommended_(this.mode);
    }

    override disconnectedCallback() {
        super.disconnectedCallback();
        document.removeEventListener('click', this.onOutsideClick_);
    }

    private updateRecommended_(mode: string) {
        if (mode === 'rpc') {
            this.recommendedGateways_ = [
                'https://api.devnet.solana.com',
                'https://devnet.helius-rpc.com/?api-key=87903272-8292-4dbd-b3c0-c9ddec0f3ef6',
            ];
        } else {
            // ipfs
            this.recommendedGateways_ = [
                'https://ipfs.io',
                'http://116.202.49.39'
            ];
        }
    }

    private resetInput_() {
        const prefValue =
            this.pref && this.pref.value !== undefined
                ? this.pref.value
                : '';

        if (this.bindingValue_ !== prefValue) {
            this.bindingValue_ = prefValue as string;
        }
    }

    private onInputChange_() {
        if (!this.pref) {
            return;
        }

        if (this.bindingValue_ !== this.pref.value) {
            this.set('pref.value', this.bindingValue_);
            this.dispatchEvent(new CustomEvent(
                'settings-control-change',
                {bubbles: true, composed: true}));
        }
    }

    private shouldDisable_(): boolean {
        return this.disabled || this.isPrefEnforced();
    }

    private toggleDropdown_(e: Event) {
        e.stopPropagation();
        this.dropdownOpen_ = !this.dropdownOpen_;
    }

    private onSelectRecommendation_(e: any) {
        const value = e.model.item;
        this.bindingValue_ = value;
        this.dropdownOpen_ = false;
        this.onInputChange_();
    }

    private onOutsideClick_ = (e: Event) => {
        if (!this.shadowRoot) {
            return;
        }

        const path = e.composedPath();
        if (!path.includes(this)) {
            this.dropdownOpen_ = false;
        }
    };
}

declare global {
    interface HTMLElementTagNameMap {
        'settings-input': SettingsInputElement;
    }
}

customElements.define(SettingsInputElement.is, SettingsInputElement);