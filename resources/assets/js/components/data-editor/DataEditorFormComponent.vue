<template>
    <component class="data-form"
        :is="component_type"
        :type="type"
        :name="name"
        :value="value"
        :options="options"
        :readonly="!focused"
        @input="emitInput"
        @keydown.esc="focused = false"
        @focus="focused = true"
        @blur="focused = false">
    </component>
</template>

<script>
export default {
    props: {
        type: String,
        name: String,
        value: null,
        options: Object,
    },
    data() { return {
        focused: false,
        modified: false,
    }},
    computed: {
        component_type() {
            return this.type == 'textarea' ? 'data-editor-textarea'
                : (this.type == 'select' ? 'data-editor-select' : 'data-editor-input');
        },
        is_modified() { return (this.modified || []).includes(this.name); }
    },
    methods: {
        emitInput(new_val) {
            this.$emit('input', new_val, this.name);
        },
        emitPush(new_val) {
            this.$emit('push', new_val, this.name);
        }
    }
}
</script>
