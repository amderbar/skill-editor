<template>
    <nav id="side_menu" v-show="nav_show">

        <header class="with-btns">
            <h2>Projects</h2>
            <ul class="btns">
                <li title="新規作成"><a href="#" id="new-proj" class="icon-database btn" @click="is_show_new_proj_form = true"></a></li>
            </ul>
        </header>

        <ul class="side-menu" id="proj-list">
            <li v-for="proj in projList" :key="proj.proj_id">
                <!-- プロジェクト名 -->
                <div class="with-btns">
                    <h2><a :href="`main?pid=${proj.proj_id}`">{{ proj.proj_name }}</a></h2>
                    <form action="/skill_editor" method="POST">
                        <input name="_token" :value="token" type="hidden" v-if="token">
                        <input name="pid" :value="proj.proj_id" type="hidden">
                        <ul class="btns">
                            <li class="icon-folder-plus btn submit" title="テーブルの追加" @click="addTbl"></li>
                            <li class="icon-bin btn submit" title="プロジェクトの削除" @click="deleteProj"></li>
                        </ul>
                    </form>
                </div>
                <!-- プロジェクトのテーブルリスト -->
                <ul class="side-children" v-if="proj.proj_id == focus">
                    <li v-for="tbl in tblList" :key="tbl.tbl_id">
                        <span class="with-btns">
                            <a :href="`editor/data/${proj.proj_id}?tab=${tbl.tbl_id}`"
                                target="editor_area"
                                @click="changeFocus(tbl.tbl_id)"
                            >{{ tbl.tbl_name }}</a>
                            <ul class="btns"><li class="icon-bin btn" title="テーブルの削除"></li></ul>
                        </span>
                    </li>
                    <li v-if="add_tbl" v-cloak><strong>untitled</strong></li>
                </ul>
            </li>
            <new-proj-form v-if="is_show_new_proj_form" @cancel="is_show_new_proj_form = false" v-cloak></new-proj-form>
            <!-- <li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>-->
        </ul>

    </nav>
</template>

<script>
    export default {
        created() {
            this.$on('show', function(){ this.nav_show = true; });
            this.$on('hide', function(){ this.nav_show = false; });
        },
        props: {
            token: String,
            projList: Array,
            tblList: Array,
            focus: Number
        },
        data() {
            return {
                nav_show: true,
                is_show_new_proj_form : false,
            }
        },
        computed: {
            add_tbl() { return this.$parent.add_tbl; }
        },
        methods: {
            addTbl(event) {
                var proj_id = $(event.target).closest('form').find('input[name="pid"]').val();
                this.$parent.$emit('add-tbl', proj_id);
            },
            deleteProj(event) {
                var $form = $(event.target).closest('form');
                var proj_name = $form.prev().text();
                if (confirm(`プロジェクト「${proj_name}」を削除してもよろしいですか？`)) {
                    var action = $form.attr('action');
                    $form.attr('action', action + '/delete').submit();
                }
            },
            changeFocus(tab_id) { this.$parent.focus_tbl = tab_id; }
        }
    }
</script>
