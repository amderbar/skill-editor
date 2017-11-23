<template>
    <button type="button" :title="title" id="toggle-menu" @click="navToggle">{{ content }}</button>  
</template>

<script>
    export default {
        mounted() {
            this.$emit(this.navDefaultShow ? 'show' : 'hide');
        },
        props: {
            navDefaultShow: {
                type: Boolean,
                default: true
            }
        },
        data() {
            return {
                navShow: this.navDefaultShow
            };
        },
        computed: {
            content() { return (this.navShow) ? '<<' : '>>'; },
            title() {  return 'サイドメニューを' + ((this.navShow) ? '閉じる' : '開く'); }
        },
        methods: {
            navToggle() {
                this.navShow = !this.navShow;
                this.$emit(this.navShow ? 'show' : 'hide');
            }
        }
    }
</script>
