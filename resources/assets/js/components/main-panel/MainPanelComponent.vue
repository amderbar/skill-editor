<template>
    <main id="main-panel">
        <section>
            <header id="tab-bar">
                <ul class="editor-tabs">
                    <li v-for="tbl in tblList"
                        :key="tbl.tbl_id"
                        :class="{editting: focus_tbl == tbl.tbl_id}"
                        @click="focus_tbl = tbl.tbl_id"
                    ><a :href="`editor/data/${focus_pid}?tab=${tbl.tbl_id}`"
                        target="editor_area"
                    >{{ tbl.tbl_name }}</a>
                    </li>
                    <li class="editting" v-if="func == 'table'" v-cloak><strong>untitled</strong></li>
                </ul>
            </header>
            <iframe class="tab-page" name="editor_area" v-if="!!focus_pid" :src="editor_focus" scrolling="no" frameborder="no"></iframe>
            <div v-else><slot></slot></div>
        </section>
    </main>
</template>

<script>
    export default {
        created() {
            this.$on('add-tbl', function(pid){
                this.func = 'table'
                this.focus_pid = pid;
                this.focus_tbl = null;
            });
        },
        mounted() {
            if (!this.tblList.length) {
                this.$emit('add-tbl', this.pid);
            }
        },
        props: {
            tblList: Array,
            pid: Number,
            tbl: Number
        },
        data() {
            return {
                func: 'data',
                focus_pid: this.pid,
                focus_tbl: this.tbl
            };
        },
        computed: {
            editor_focus() {
                let query = '';
                if (this.focus_pid && this.focus_tbl) {
                    query += `?tab=${this.focus_tbl}`;
                }
                return `editor/${this.func}/${this.focus_pid}${query}`;
            },
        }

    }
</script>
