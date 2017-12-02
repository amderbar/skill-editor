<template>
    <th class="col-h"
        @click="emitClick($event)"
        @mouseenter="on_hover = true"
        @mouseleave="on_hover = false"
    >
        <slot></slot>
        <label>列</label>
        <span v-show="on_hover && !isOnly">削除</span>
    </th>
</template>

<script>
export default {
    props: {
        isOnly: Boolean
    },
    data() { return {
        on_hover: false
    }},
    methods: {
        emitClick(event) {
            if (!this.isOnly) {
                this.$emit('click', event);
            }
        }
    }
}
</script>
