<template>
    <select :name="name"
           :value="value"
           @change="emitChange($event.target.value)"
           @keydown.enter.prevent="emitEvent('keydown-enter', $event)"
           @keydown.tab.stop="emitEvent('keydown-tab', $event)"
           @keydown.esc="emitEvent('keydown-esc', $event)"
           @focus="$emit('focus', $event)"
           @blur="emitEvent('blur', $event)">
        <option v-for="(txt, value) in options"
                :key="value"
                :value="value">
            {{ txt }}
        </option>
    </select>
</template>

<script>
// import formComponentMixin from './FormComponentMixin.js'
export default {
    props: {
        name: String,
        value: null,
        options: Object,
    },
}
</script>
