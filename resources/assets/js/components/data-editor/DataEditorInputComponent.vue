<template>
    <input :type="type"
        :name="name"
        :value="value"
        @change="$emit('input', $event.target.value)"
        @keydown.enter.prevent="$emit('keydown-enter', $event)"
        @keydown.tab.stop="$emit('keydown-tab', $event)"
        @keydown.esc="$emit('keydown-esc', $event)"
        @focus="$emit('focus', $event)"
        @blur="$emit('blur', $event)">
</template>

<script>
export default {
    props: {
        type: String,
        name: String,
        value: null,
    },
}
</script>
