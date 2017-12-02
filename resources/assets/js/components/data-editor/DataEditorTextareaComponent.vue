<template>
    <textarea :name="name"
        :value="value"
        @change="emitChange($event.target.value)"
        @keydown.enter.prevent="emitEvent('keydown-enter', $event)"
        @keydown.tab.stop="emitEvent('keydown-tab', $event)"
        @keydown.esc="emitEvent('keydown-esc', $event)"
        @focus="$emit('focus', $event)"
        @blur="emitEvent('blur', $event)"></textarea>
</template>

<script>
export default {
    props: {
        name: String,
        value: null,
    },
}
</script>
